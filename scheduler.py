from apscheduler.schedulers.background import BackgroundScheduler
import asyncio
from state import State

sched = BackgroundScheduler()
sched.start()


# single scheduled
def add_single_brew_job(start_date: int, duration: int, state):
    sched.add_job(trigger_brew, "date", [duration, state], run_date=start_date)


# scheduled brew job to repeat
def add_brew_job(day: int, hour: str, minute: str, duration: int, state):
    sched.add_job(
        trigger_brew,
        "cron",
        [duration, state],
        day_of_week=day,
        hour=hour,
        minute=minute,
    )


# clear state
def remove_all_jobs():
    sched.remove_all_jobs()


# action to run when a brew job is ready
def trigger_brew(duration: int, state):
    state.action = "start"
    asyncio.sleep(duration)
    state.action = "stop"
