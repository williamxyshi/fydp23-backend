from apscheduler.schedulers.asyncio import AsyncIOScheduler
import asyncio
from state import State
from datetime import datetime

sched = AsyncIOScheduler()
sched.start()


# single scheduled
async def add_single_brew_job(
    ready_timestamp: int, duration: int, strength: str, size: str, state: State
):
    start_date = datetime.fromtimestamp(ready_timestamp - duration)
    sched.add_job(
        trigger_brew, "date", [duration, strength, size, state], run_date=start_date
    )


# scheduled brew job to repeat
async def add_scheduled_brew_job(
    day: int, ready_time: str, duration: int, strength: str, size: str, state: State
):
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
    sched.remove_all_jobs()


# action to run when a brew job is ready
async def trigger_brew(duration: int, strength: str, size: str, state: State):
    # tell arduino to run dispense/pump process
    state.action = "go"
    state.strength = strength
    state.size = size

    # wait for dispense/pump process to finish
    await state.start_event.wait()

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

    # hey we're done the brew!
    state.action = "wait"
    state.start_event.clear()
    state.finish_event.clear()
