from helpers import get_num_dispenses, get_pump_time_ms
import asyncio


class State:
    def __init__(self):
        self.action: str = "wait"
        self.strength: str | None = None
        self.size: str | None = None
        self.start_event = asyncio.Event()
        self.finish_event = asyncio.Event()

    def num_dispenses(self):
        return get_num_dispenses(self.strength, self.size)

    def pump_time_ms(self):
        return get_pump_time_ms(self.size)
