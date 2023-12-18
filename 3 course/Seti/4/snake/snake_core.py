import numpy as np
from threading import Lock
import snakes_pb2 as snkpt

directions_map = {(0, -1): snkpt.Direction.UP, (0, 1): snkpt.Direction.DOWN, (1, 0): snkpt.Direction.RIGHT,
                  (-1, 0): snkpt.Direction.LEFT}

directions_map_rev = {snkpt.Direction.UP: (0, -1), snkpt.Direction.DOWN: (0, 1), snkpt.Direction.RIGHT: (1, 0),
                      snkpt.Direction.LEFT: (-1, 0)}


def send_direction(my_socket, addr, my_id, direction):
    print(addr)
    send_msg = snkpt.GameMessage()
    send_msg.SteerMsg()
    send_msg.msg_seq = 0
    send_msg.sender_id = my_id
    send_msg.receiver_id = 0
    send_msg.steer.direction = directions_map[direction]  # direction

    my_socket.sendto(send_msg.SerializeToString(), addr)


class Snake:
    def __init__(self, x, y, color):
        self.head = (x, y)
        self.color = color
        self.body_shift = []
        self.direction = (1, 0)
        self.is_alive = True

    def get_from_state(self, snake_state):
        self.head = (snake_state.points[0].x, snake_state.points[0].y)
        self.body_shift = [(snake_state.points[i].x, snake_state.points[i].y)
                           for i in range(1, len(snake_state.points))]
        self.direction = directions_map_rev[snake_state.head_direction]
        self.is_alive = snake_state.state == snkpt.GameState.Snake.SnakeState.ALIVE

    def get_state(self):
        a = (self.head, *self.body_shift)
        snake_state = snkpt.GameState.Snake()
        snake_state.player_id = 0
        snake_state.points.extend([snkpt.GameState.Coord(x=x, y=y) for x, y in a])
        snake_state.state = snkpt.GameState.Snake.SnakeState.ALIVE if self.is_alive \
            else snkpt.GameState.Snake.SnakeState.ZOMBIE
        snake_state.head_direction = directions_map[self.direction]
        return snake_state

    def is_turn_possible(self, new_dir):
        x = self.direction[0] * new_dir[0]
        y = self.direction[1] * new_dir[1]
        if x + y == -1:
            return False
        return True

    def turn_right(self):
        if self.is_turn_possible((1, 0)):
            self.direction = (1, 0)

    def turn_left(self):
        if self.is_turn_possible((-1, 0)):
            self.direction = (-1, 0)

    def turn_up(self):
        if self.is_turn_possible((0, -1)):
            self.direction = (0, -1)

    def turn_down(self):
        if self.is_turn_possible((0, 1)):
            self.direction = (0, 1)

    def turn(self, direction):

        if self.is_turn_possible(direction):
            self.direction = direction

    def append_body(self):
        if self.body_shift:
            self.body_shift.append(self.body_shift[-1])
        else:
            self.body_shift.append(self.direction)

    def move(self, x_pixels, y_pixels):
        old_head = self.head
        self.head = ((self.head[0] + self.direction[0]) % x_pixels, (self.head[1] + self.direction[1]) % y_pixels)
        if self.body_shift:
            new_direction = [(old_head[0] - self.head[0], old_head[1] - self.head[1])]
            for i in range(1, len(self.body_shift)):
                new_direction.append(self.body_shift[i - 1])
            self.body_shift = new_direction


# class SnakeOnline(Snake):
#     def __init__(self, x, y, color, connection):
#         self.connection = connection
#         super().__init__(x, y, color)
#
#     def turn_right(self):
#         self.connection.send_steer_message(self.connection, snkpt.Direction.RIGHT)
#
#     def turn_left(self):
#         self.connection.send_steer_message(self.connection, snkpt.Direction.LEFT)
#
#     def turn_up(self):
#         self.connection.send_steer_message(self.connection, snkpt.Direction.UP)
#
#     def turn_down(self):
#         self.connection.send_steer_message(self.connection, snkpt.Direction.DOWN)
#
#
# class SnakeOffline(Snake):
#
#     def __init__(self, x, y, color):
#         super().__init__(x, y, color)


class Field:

    def __init__(self):
        self.x_pixels = 40
        self.y_pixels = 30
        self.pixel_size = 25
        self.snakes = {}
        self.food = set([])
        self.mutex = Lock()
        self.id_buffer = 0

    def get_dots(self, snake):
        dots = [snake.head]
        for i in snake.body_shift:
            dots.append(((dots[-1][0] + i[0]) % self.x_pixels, (dots[-1][1] + i[1]) % self.y_pixels))
        return dots

    def add_snake(self, snake):
        with self.mutex:
            if not self.snakes:
                self.snakes[0] = snake
                return 0
            new_id = max(self.snakes.keys()) + 1
            self.snakes[new_id] = snake
            return new_id

    def calculate_collides(self):

        dots = {}

        to_delete = set()

        for index, snake in self.snakes.items():
            if not snake.is_alive:
                continue

            for i in self.get_dots(snake):
                if i in dots:
                    if i == snake.head and self.snakes[dots[i]].head == i:
                        to_delete.add(index)
                        to_delete.add(dots[i])
                    elif self.snakes[dots[i]].head == i:
                        to_delete.add(dots[i])
                    else:
                        to_delete.add(index)
                    continue
                dots[i] = index
        return to_delete

    def move(self):
        with self.mutex:
            i = 0
            for index, snake in self.snakes.items():
                if snake.is_alive:
                    snake.move(self.x_pixels, self.y_pixels)
                    if snake.head in self.food:
                        snake.append_body()
                        self.food.remove(snake.head)
                        i += 1

            to_delete = self.calculate_collides()

            for index in to_delete:
                self.snakes[index].is_alive = False
                # self.snakes.pop(index)

            for j in range(i):
                self.spawn_food()

    def spawn_food(self):
        n = np.random.randint(0, self.x_pixels * self.y_pixels)
        field = np.zeros((self.x_pixels, self.y_pixels))

        for _, snake in self.snakes.items():
            if snake.is_alive:
                for i, j in self.get_dots(snake):
                    field[i][j] = 1

        for i in self.food:
            field[i[0]][i[1]] = 1

        while n > 0:
            for i in range(self.x_pixels):
                for j in range(self.y_pixels):
                    if field[i][j] == 0:
                        n -= 1
                        if n == 0:
                            self.food.add((i, j))
