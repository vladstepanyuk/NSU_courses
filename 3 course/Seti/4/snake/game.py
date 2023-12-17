import threading
import time

import snakes_pb2 as snkpt
import snake_core as sc
import socket

from draw_utils import *

from concurrent.futures import ThreadPoolExecutor

white = (255, 255, 255)
black = (0, 0, 0)
red = (255, 0, 0)
green = (0, 255, 0)

state_delay_sec = 1


class IdBuffer:
    def __init__(self):
        self.id = 0
        self.mutex = threading.Lock()

    def get_id(self):
        with self.mutex:
            self.id += 1
            return self.id


id_buffer = IdBuffer()


def disconnect_checker(my_id, my_socket, field, clients, stop):
    while not stop.is_set():
        # print(1)
        to_remove = []
        for _, client_time in clients.items():
            if client_time[0].id == my_id:
                continue

            client = client_time[0]
            time_last = client_time[1]

            if time.monotonic() - time_last > state_delay_sec * 0.8:
                to_remove.append(client.id)
            elif time.monotonic() - time_last > state_delay_sec / 10:
                send_msg = snkpt.GameMessage()
                send_msg.PingMsg()
                send_msg.msg_seq = 0
                send_msg.sender_id = 1
                send_msg.receiver_id = client.id

                my_socket.sendto(send_msg.SerializeToString(), (client.ip_address, client.port))

        with field.mutex:
            for i in to_remove:
                if i in field.snakes:
                    field.snakes.pop(i)
                clients.pop(i)
        time.sleep(0.5)


def add_new_snake(my_id, my_socket, addr, field, clients):
    new_id = field.add_snake(sc.SnakeOffline(0, 0, black))
    send_msg = snkpt.GameMessage()
    send_msg.AckMsg()
    send_msg.msg_seq = 0
    send_msg.sender_id = 1
    send_msg.receiver_id = new_id
    client = snkpt.GamePlayer()
    client.ip_address = addr[0]
    client.port = addr[1]
    client.id = new_id
    client.name = "snake"
    client.role = snkpt.NORMAL
    client.score = 0

    clients[new_id] = [client, time.monotonic()]

    my_socket.sendto(send_msg.SerializeToString(), addr)

    if len(clients) == 2:
        make_new_deputy(new_id, my_socket, clients)


def make_new_deputy(my_id, my_socket, clients):
    new_deputy = None
    for client in clients.values():
        if client[0].id != my_id:
            new_deputy = client[0]
            send_msg = snkpt.GameMessage()
            send_msg.RoleChangeMsg()
            send_msg.msg_seq = id_buffer.get_id()
            send_msg.sender_id = my_id
            send_msg.receiver_id = client[0].id
            send_msg.role_change.sender_role = snkpt.MASTER
            send_msg.role_change.receiver_role = snkpt.DEPUTY
            break
    if new_deputy is None:
        return
    print(1)
    my_socket.sendto(send_msg.SerializeToString(), (new_deputy.ip_address, new_deputy.port))


def socket_listener_server(my_id, my_socket, field, clients, executor, stop):
    my_socket.settimeout(0.5)

    while not stop.is_set():
        try:
            data, addr = my_socket.recvfrom(1024)
        except socket.timeout:
            continue
        game_msg = snkpt.GameMessage()
        game_msg.ParseFromString(data)
        if game_msg.sender_id in clients:
            clients[game_msg.sender_id][1] = time.monotonic()
        if game_msg.join.IsInitialized():
            executor.submit(add_new_snake, my_id, my_socket, addr, field, clients)
        elif game_msg.steer.IsInitialized():
            field.snakes[game_msg.sender_id].turn(game_msg.steer.direction)


def get_state(field, clients):
    with field.mutex:
        field_copy = field
    state_msg = snkpt.GameMessage()
    state_msg.StateMsg()
    state_msg.state.state.state_order = 0
    state_msg.msg_seq = 0
    state_msg.sender_id = 1
    for id, snake in field_copy.snakes.items():
        snake_prt = snake.get_state()
        snake_prt.player_id = id
        state_msg.state.state.snakes.append(snake_prt)
    for x, y in field_copy.food:
        cord = snkpt.GameState.Coord()
        cord.x = x
        cord.y = y
        state_msg.state.state.foods.append(cord)
    state_msg.state.state.players.CopyFrom(snkpt.GamePlayers())
    # print(clients)
    for _, client_time in clients.items():
        state_msg.state.state.players.players.append(client_time[0])
    return state_msg


def send_state(my_id, my_socket, field, clients, stop):
    while not stop.is_set():
        if len(clients) == 1:
            time.sleep(0.5)
            continue
        state = get_state(field, clients)

        for _, client_time in clients.items():
            if client_time[0].id == my_id:
                continue
            state_for_this = snkpt.GameMessage()
            state_for_this.CopyFrom(state)
            id_msg = id_buffer.get_id()
            state_for_this.msg_seq = id_msg
            state_for_this.receiver_id = client_time[0].id
            my_socket.sendto(state_for_this.SerializeToString(), (client_time[0].ip_address, client_time[0].port))
        time.sleep(0.05)


def socket_listener_client(my_socket, my_id, field, stop):
    my_socket.settimeout(0.5)

    while not stop.is_set():
        try:
            data, addr = my_socket.recvfrom(1024)
        except socket.timeout:
            continue
        game_msg = snkpt.GameMessage()
        game_msg.ParseFromString(data)
        if game_msg.state.state.IsInitialized():
            with field.mutex:
                field.food = set()
                for cord in game_msg.state.state.foods:
                    field.food.add((cord.x, cord.y))
                field.snakes.clear()
                for snake_state in game_msg.state.state.snakes:
                    field.snakes[snake_state.player_id] = sc.SnakeOffline(0, 0, black)
                    field.snakes[snake_state.player_id].get_from_state(snake_state)
        if game_msg.ping.IsInitialized():
            send_msg = snkpt.GameMessage()
            send_msg.AckMsg()
            send_msg.msg_seq = 0
            send_msg.receiver_id = 1
            send_msg.sender_id = my_id
            my_socket.sendto(send_msg.SerializeToString(), addr)
        if (game_msg.role_change.IsInitialized() and game_msg.role_change.receiver_role == snkpt.DEPUTY
                and game_msg.role_change.sender_role == snkpt.MASTER):
            print("New deputy")



def server_game(server_port):
    my_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    my_socket.bind(("127.0.0.1", server_port))

    print(f"Server IP: {my_socket.getsockname()}")

    field = sc.Field()

    my_id = 0

    me = snkpt.GamePlayer()
    me.id = my_id
    me.name = "admin"
    me.ip_address = my_socket.getsockname()[0]
    me.port = my_socket.getsockname()[1]
    me.role = snkpt.MASTER
    me.score = 0
    me.type = snkpt.HUMAN

    clients = {0: [me, -1]}

    # message_pool = {}

    stop = threading.Event()

    executor = ThreadPoolExecutor(max_workers=10)

    listening_thread = threading.Thread(target=socket_listener_server,
                                        args=(my_id, my_socket, field, clients, executor, stop))
    listening_thread.start()
    send_state_thread = threading.Thread(target=send_state, args=(my_id, my_socket, field, clients, stop))
    send_state_thread.start()

    disconnect_checker_thread = threading.Thread(target=disconnect_checker,
                                                 args=(my_id, my_socket, field, clients, stop))
    disconnect_checker_thread.start()

    snake = sc.SnakeOffline(0, 0, black)

    x_pixels = field.x_pixels * field.pixel_size
    y_pixels = field.y_pixels * field.pixel_size

    game_over = False

    dis = pygame.display.set_mode((x_pixels, y_pixels))
    pygame.display.set_caption('Snake game by Pythonist')

    clock = pygame.time.Clock()

    field.add_snake(snake)

    field.spawn_food()

    while not game_over:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                game_over = True
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_LEFT:
                    snake.turn_left()
                elif event.key == pygame.K_RIGHT:
                    snake.turn_right()
                elif event.key == pygame.K_UP:
                    snake.turn_up()
                elif event.key == pygame.K_DOWN:
                    snake.turn_down()

        field.move()

        dis.fill(white)
        draw_field(field, dis)

        pygame.display.update()

        clock.tick(10)

    stop.set()
    listening_thread.join()
    send_state_thread.join()
    executor.shutdown(wait=True)
    my_socket.close()
    pygame.quit()
    # quit()


def client_game(server_ip, server_port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(("127.0.0.1", 0))

    print(f"Client IP: {sock.getsockname()}")

    addr = (server_ip, server_port)

    field = sc.Field()

    stop = threading.Event()

    join_msg = snkpt.GameMessage()
    join_msg.msg_seq = 0
    join_msg.sender_id = 1
    join_msg.receiver_id = 2
    join_msg.JoinMsg()
    join_msg.join.game_name = "Snake"
    join_msg.join.player_name = "Pythonist"
    join_msg.join.player_type = snkpt.HUMAN
    join_msg.join.requested_role = snkpt.NORMAL
    sock.sendto(join_msg.SerializeToString(), (server_ip, server_port))

    data, _ = sock.recvfrom(1024)

    received_msg = snkpt.GameMessage()
    received_msg.ParseFromString(data)
    my_id = received_msg.receiver_id

    reciever_thread = threading.Thread(target=socket_listener_client, args=(sock, my_id, field, stop))
    reciever_thread.start()

    x_pixels = field.x_pixels * field.pixel_size
    y_pixels = field.y_pixels * field.pixel_size

    dis = pygame.display.set_mode((x_pixels, y_pixels))
    pygame.display.set_caption('Snake game by Pythonist')

    clock = pygame.time.Clock()

    game_over = False

    while not game_over:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                game_over = True
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_LEFT:
                    sc.send_direction(sock, addr, my_id, snkpt.Direction.LEFT)
                elif event.key == pygame.K_RIGHT:
                    sc.send_direction(sock, addr, my_id, snkpt.Direction.RIGHT)
                elif event.key == pygame.K_UP:
                    sc.send_direction(sock, addr, my_id, snkpt.Direction.UP)
                elif event.key == pygame.K_DOWN:
                    sc.send_direction(sock, addr, my_id, snkpt.Direction.DOWN)

        dis.fill(white)
        draw_field(field, dis)

        pygame.display.update()

        clock.tick(10)

    stop.set()
    reciever_thread.join()
    sock.close()
    pygame.quit()
    # quit()
