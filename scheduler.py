from apscheduler.schedulers.asyncio import AsyncIOScheduler
import asyncio
from state import State
from datetime import datetime
from math import floor

sched = AsyncIOScheduler()
sched.start()


# single scheduled
async def add_single_brew_job(ready_timestamp, duration, strength, size, state):
    print("received single brew job!")
    start_date = datetime.fromtimestamp(ready_timestamp - duration)
    sched.add_job(
        trigger_brew, "date", [duration, strength, size, state], run_date=start_date
    )


def get_day(id):
    days = [
        "monday",
        "tuesday",
        "wednesday",
        "thursday",
        "friday",
        "saturday",
        "sunday",
    ]
    return days[id]


# scheduled brew job to repeat
async def add_scheduled_brew_job(day, ready_time, duration, strength, size, state):
    print(f"received scheduled brew job for {get_day(day)}!")
    hour, minute = ready_time.split(":")  # 4:20 -> 4, 20
    sched.add_job(
        trigger_brew,
        "cron",
        [duration, strength, size, state],
        day_of_week=day,
        hour=hour,
        minute=minute,
    )


# clear state
def remove_all_jobs():
    print("clearing scheduler!")
    sched.remove_all_jobs()


# action to run when a brew job is ready
async def trigger_brew(duration, strength, size, state):
    # reset values here
    state.start_timestamp = None
    state.finish_timestamp = None
    state.is_done = False
    state.start_event.clear()
    state.start_event.clear()

    # tell arduino to run dispense/pump process
    state.action = "go"
    state.duration = duration
    state.strength = strength
    state.size = size

    # wait for dispense/pump process to finish
    await state.start_event.wait()

    # save start time for frontend
    state.start_timestamp = floor(datetime.now().timestamp())
    state.is_brewing = True

    # and now we wait for the duration to complete :P
    state.action = "wait"
    state.strength = None
    state.size = None

    # delay for x seconds
    await asyncio.sleep(duration)

    # tell arduino to run the press process
    state.action = "stop"

    # wait for press process to finish
    await state.finish_event.wait()

    # save finish time for frontend
    state.finish_timestamp = floor(datetime.now().timestamp())
    state.is_brewing = False
    state.is_done = True
    state.duration = None

    # hey we're done the brew!
    state.action = "wait"
    state.start_event.clear()
    state.finish_event.clear()
