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


class TurnFunc:
    def __init__(self, func, *always_args):
        self.mutex = threading.Lock()
        self.func = func
        self.always_args = always_args

    def __call__(self, *args, **kwargs):
        return self.func(*self.always_args, *args, **kwargs)

    def set_func(self, func, *always_args):
        with self.mutex:
            self.func = func
            self.always_args = always_args


class Context:

    def __init__(self, my_id, my_socket, field, clients, stop, turn_func, is_master):
        self.mutex = threading.Lock()
        self.my_id = my_id
        self.my_socket = my_socket
        self.field = field
        self.clients = clients
        self.stop = stop
        self.turn_func = turn_func
        self.is_master = is_master
        self.send_thread = None


class IdBuffer:
    def __init__(self):
        self.id = 0
        self.mutex = threading.Lock()

    def get_id(self):
        with self.mutex:
            self.id += 1
            return self.id


id_buffer = IdBuffer()


def first_match(iterable, predicate):
    try:
        return next(n for n in iterable if predicate(n))
    except StopIteration:
        return None


def disconnect_checker(context):
    while not context.stop.is_set():
        to_remove = []
        for _, client_time in context.clients.items():
            if client_time[0].id == context.my_id:
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

                context.my_socket.sendto(send_msg.SerializeToString(), (client.ip_address, client.port))

        is_master_deleted = False
        is_deputy_deleted = False

        # if context.my_id in context.clients:
        #     print(context.clients[context.my_id][0].role)

        with context.field.mutex:
            for i in to_remove:
                # print(context.clients[i][0])
                # print(context.clients[context.my_id][0])
                if context.clients[i][0].role == snkpt.MASTER:
                    is_master_deleted = True
                elif context.clients[i][0].role == snkpt.DEPUTY:
                    is_deputy_deleted = True
                context.clients.pop(i)

        with context.mutex:
            if is_master_deleted and context.clients[context.my_id][0].role == snkpt.DEPUTY:
                context.clients[context.my_id][0].role = snkpt.MASTER
                context.turn_func = TurnFunc(lambda snake, direction: snake.turn(direction),
                                             context.field.snakes[context.my_id])
                context.is_master = True
                send_state_thread = threading.Thread(target=send_state, args=(
                    context.my_id, context.my_socket, context.field, context.clients, context.stop))
                send_state_thread.start()
                context.send_thread = send_state_thread

                if len(context.clients) > 1:
                    make_new_deputy(context.my_id, context.my_socket, context.clients)

            elif is_master_deleted and context.clients[context.my_id][0].role == snkpt.NORMAL:
                i = None
                # print(1111111111)

                for _, client_time in context.clients.items():
                    if client_time[0].role == snkpt.DEPUTY or client_time[0].role == snkpt.MASTER:
                        i = client_time[0]
                        break
                # print(i)
                # print(context.clients)
                if i is not None:
                    context.turn_func = TurnFunc(sc.send_direction, context.my_socket,
                                                 (i.ip_address, i.port),
                                                 context.my_id)
            elif context.is_master and is_deputy_deleted and len(context.clients) > 1:
                make_new_deputy(context.my_id, context.my_socket, context.clients)
        time.sleep(0.5)


def add_new_snake(context, addr):
    new_id = context.field.add_snake(sc.Snake(0, 0, black))
    # send_msg = None
    send_msg = snkpt.GameMessage()
    send_msg.AckMsg()
    send_msg.msg_seq = id_buffer.get_id()
    send_msg.sender_id = 0
    send_msg.receiver_id = new_id
    print(send_msg)

    client = snkpt.GamePlayer()
    client.ip_address = addr[0]
    client.port = addr[1]
    client.id = new_id
    client.name = "snake"
    client.role = snkpt.NORMAL
    client.score = 0

    context.clients[new_id] = [client, time.monotonic()]

    context.my_socket.sendto(send_msg.SerializeToString(), addr)

    if len(context.clients) == 2:
        make_new_deputy(context.my_id, context.my_socket, context.clients)


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
    new_deputy.role = snkpt.DEPUTY
    print(send_msg)
    my_socket.sendto(send_msg.SerializeToString(), (new_deputy.ip_address, new_deputy.port))


def socket_listener_server(context, executor):
    context.my_socket.settimeout(0.5)

    while not context.stop.is_set():
        try:
            data, addr = context.my_socket.recvfrom(1024)
        except socket.timeout:
            continue
        game_msg = snkpt.GameMessage()
        game_msg.ParseFromString(data)
        if game_msg.sender_id in context.clients:
            context.clients[game_msg.sender_id][1] = time.monotonic()
        if game_msg.join.IsInitialized():
            executor.submit(add_new_snake, context, addr)
        elif game_msg.steer.IsInitialized():
            context.field.snakes[game_msg.sender_id].turn(sc.directions_map_rev[game_msg.steer.direction])
        elif game_msg.state.state.IsInitialized():
            with context.field.mutex:
                context.field.food = set()
                for cord in game_msg.state.state.foods:
                    context.field.food.add((cord.x, cord.y))
                context.field.snakes.clear()
                for snake_state in game_msg.state.state.snakes:
                    context.field.snakes[snake_state.player_id] = sc.Snake(0, 0, black)
                    context.field.snakes[snake_state.player_id].get_from_state(snake_state)
                context.clients.clear()
                with context.mutex:
                    for client in game_msg.state.state.players.players:
                        context.clients[client.id] = [client, time.monotonic()]
        elif game_msg.ping.IsInitialized():
            send_msg = snkpt.GameMessage()
            send_msg.AckMsg()
            send_msg.msg_seq = id_buffer.get_id()
            send_msg.sender_id = context.my_id
            context.my_socket.sendto(send_msg.SerializeToString(), addr)
        elif (game_msg.role_change.IsInitialized() and game_msg.role_change.receiver_role == snkpt.DEPUTY
              and game_msg.role_change.sender_role == snkpt.MASTER):
            print("New deputy")


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


def server_game(is_master):
    my_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    my_socket.bind(("127.0.0.1", 0))

    print(f"ADDRESS: {my_socket.getsockname()}")

    field = sc.Field()
    stop = threading.Event()

    if is_master:
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
    else:
        ip_master = input("Enter ip of master: ")
        port_master = int(input("Enter port of master: "))
        addr = (ip_master, port_master)

        join_msg = snkpt.GameMessage()
        join_msg.msg_seq = 0
        join_msg.JoinMsg()
        join_msg.join.game_name = "Snake"
        join_msg.join.player_name = "Pythonist"
        join_msg.join.player_type = snkpt.HUMAN
        join_msg.join.requested_role = snkpt.NORMAL
        my_socket.sendto(join_msg.SerializeToString(), addr)

        data, _ = my_socket.recvfrom(1024)

        received_msg = snkpt.GameMessage()
        received_msg.ParseFromString(data)
        my_id = received_msg.receiver_id
        print(received_msg)
        clients = {}

    if is_master:
        snake = sc.Snake(0, 0, black)
        field.add_snake(snake)
        field.spawn_food()
        turn_func = TurnFunc(lambda snake, direction: snake.turn(direction), snake)
    else:
        turn_func = TurnFunc(sc.send_direction, my_socket, addr, my_id)

    executor = ThreadPoolExecutor(max_workers=10)

    context = Context(my_id, my_socket, field, clients, stop, turn_func, is_master)

    listening_thread = threading.Thread(target=socket_listener_server,
                                        args=(context, executor))
    listening_thread.start()

    if is_master:
        send_state_thread = threading.Thread(target=send_state, args=(my_id, my_socket, field, clients, stop))
        send_state_thread.start()

    disconnect_checker_thread = threading.Thread(target=disconnect_checker,
                                                 args=(context,))
    disconnect_checker_thread.start()

    x_pixels = field.x_pixels * field.pixel_size
    y_pixels = field.y_pixels * field.pixel_size

    game_over = False

    dis = pygame.display.set_mode((x_pixels, y_pixels))
    pygame.display.set_caption('Snake game by Pythonist')

    clock = pygame.time.Clock()

    while not game_over:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                game_over = True
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_LEFT:
                    context.turn_func((-1, 0))
                elif event.key == pygame.K_RIGHT:
                    context.turn_func((1, 0))
                elif event.key == pygame.K_UP:
                    context.turn_func((0, -1))
                elif event.key == pygame.K_DOWN:
                    context.turn_func((0, 1))

        if context.is_master:
            field.move()

        dis.fill(white)
        draw_field(field, dis)

        pygame.display.update()

        clock.tick(10)

    stop.set()
    listening_thread.join()
    if is_master:
        send_state_thread.join()
    elif context.is_master:
        context.send_thread.join()

    disconnect_checker_thread.join()
    executor.shutdown(wait=True)
    my_socket.close()
    pygame.quit()
    # quit()
