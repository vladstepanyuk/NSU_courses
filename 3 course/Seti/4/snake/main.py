# from game_object import *

from game import server_game

from draw_utils import *

white = (255, 255, 255)
black = (0, 0, 0)
red = (255, 0, 0)
green = (0, 255, 0)

id_buffer = 1

server_port = 12345

stop = False

mode = input("Server or Client? (s/c): ")
pygame.init()
if mode == "s":
    server_game(True)
elif mode == "c":
    server_game(False)
    # client_game("127.0.0.1", server_port)
