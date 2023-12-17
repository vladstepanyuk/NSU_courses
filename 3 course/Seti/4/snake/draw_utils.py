import pygame

white = (255, 255, 255)
black = (0, 0, 0)
red = (255, 0, 0)
green = (0, 255, 0)


def draw_snake(snake, surface, pixel_size):
    dots = snake.get_dots()
    for i in dots:
        pygame.draw.rect(surface, snake.color, [i[0] * pixel_size, i[1] * pixel_size, pixel_size, pixel_size])


def draw_food(food, pixel_size, surface):
    for i in food:
        pygame.draw.rect(surface, red,
                         [i[0] * pixel_size, i[1] * pixel_size, pixel_size, pixel_size])


def draw_field(field, surface):
    draw_food(field.food, field.pixel_size, surface)

    for _, snake in field.snakes.items():

        for i in field.get_dots(snake):
            pygame.draw.rect(surface, snake.color, [i[0] * field.pixel_size, i[1] * field.pixel_size, field.pixel_size, field.pixel_size])
