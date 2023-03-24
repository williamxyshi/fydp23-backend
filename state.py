from helpers import get_num_dispenses, get_pump_time_ms
import asyncio


class Schedule:
    def __init__(self, days, ready_time):
        self.days = days
        self.ready_time = ready_time


class State:
    def __init__(self):
        # for the arduino
        self.action: str = "wait"

        # internal crap
        self.strength: str | None = None
        self.size: str | None = None
        self.start_event = asyncio.Event()
        self.finish_event = asyncio.Event()

        # for the app
        self.start_timestamp: int | None = None
        self.finish_timestamp: int | None = None
        self.duration: int | None = None
        self.schedule: Schedule | None = None
        self.is_brewing: bool = False
        self.is_done: bool = False
        self.is_cold: bool = True

    # the amount of times the arduino should open + close dispenser doors
    def num_dispenses(self):
        return get_num_dispenses(self.strength, self.size)

    # number of milliseconds arduino should run pump for
    def pump_time_ms(self):
        return get_pump_time_ms(self.size, self.is_cold)
