import asyncio
import logging
import sys
import aiohttp
from os import getenv
from json import load

from aiogram import Bot, Dispatcher, Router, types
from aiogram.fsm.context import FSMContext
from aiogram.fsm.state import State, StatesGroup
from aiogram.enums import ParseMode
from aiogram.filters import CommandStart
from aiogram.types import Message, ReplyKeyboardMarkup, KeyboardButton, ReplyKeyboardRemove
from aiogram.utils.markdown import hbold
from threading import Lock

context = load(open("config.json"))
TOKEN = context["token"]

# All handlers should be attached to the Router (or Dispatcher)
dp = Dispatcher()

storage = {}

mutex = Lock()

router = Router()


class Form(StatesGroup):
    name = State()
    choose_sity = State()


def form_address(location):
    return (f"country: {location['country']}, type: {location['osm_key']}, name: {location['name']}"
            f" coord: ({location['point']['lng']}, {location['point']['lat']})")

def form_weather(weather_data):
    return f"temperature: {weather_data['main']['temp']}°C, feels like: {weather_data['main']['feels_like']}°C"


@router.message(CommandStart())
async def command_start_handler(message: Message, state: FSMContext) -> None:
    await state.set_state(Form.name)

    await message.answer(f"Hello, {hbold(message.from_user.full_name)}! Please type some address.")


@router.message(Form.name)
async def echo_handler(message: types.Message, state: FSMContext) -> None:
    """
    Handler will forward receive a message back to the sender

    By default, message handler will handle all message types (like a text, photo, sticker etc.)
    """
    try:
        # Send a copy of the received message
        async with aiohttp.ClientSession() as session:
            async with session.get(
                    f"https://graphhopper.com/api/1/geocode?q={message.text}&locale=en&key={context['geo-key']}") as response:
                data = await response.json()


        buttons = []
        if len(data['hits']) == 0:
            await message.answer("No results found")
            await state.set_state(Form.name)
            return

        with mutex:
            storage[message.from_user.id] = data['hits'][:5]

        reply = ''




        for i in range(len(data['hits'][:5])):
            buttons.append(KeyboardButton(text=f"{i+1}. {data['hits'][i]['name']}"))
            reply += str(i + 1) + ". " + form_address(data['hits'][i]) + '\n\n'

        keyboard = ReplyKeyboardMarkup(
            keyboard=[
                buttons
            ],
            resize_keyboard=True,
        )

        await message.answer(reply, reply_markup=keyboard)

    except TypeError:
        # But not all the types is supported to be copied so need to handle it
        await message.answer("Nice try!")

    await state.set_state(Form.choose_sity)



@router.message(Form.choose_sity)
async def choose_sity_handler(message: types.Message, state: FSMContext) -> None:
    await state.set_state(Form.name)

    i = int(message.text.split(".")[0])

    data = storage[message.from_user.id][i - 1]

    async with aiohttp.ClientSession() as session:
        async with session.get(
                f"https://api.openweathermap.org/data/2.5/weather?lat={data['point']['lat']}&lon={data['point']['lng']}&units=metric&appid={context['weather-key']}") as response:
            data_weather = await response.json()

    # print(data_weather['main'])
    await message.answer(form_weather(data_weather), reply_markup=ReplyKeyboardRemove())

async def main() -> None:
    # Initialize Bot instance with a default parse mode which will be passed to all API calls
    bot = Bot(TOKEN, parse_mode=ParseMode.HTML)
    dp.include_router(router)
    # And the run events dispatching
    await dp.start_polling(bot)


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, stream=sys.stdout)
    asyncio.run(main())
