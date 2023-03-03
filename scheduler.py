from asyncio import QueueEmpty
from datetime import datetime, timedelta

from apscheduler.schedulers.background import BackgroundScheduler
from apscheduler.schedulers.blocking import BlockingScheduler
from sqlalchemy.orm import Session

sched: BlockingScheduler = BackgroundScheduler()
sched.start()

#single scheduled
def add_single_brew_job(start_date: int, duration: int):
    sched.add_job(trigger_brew, 'date', [duration], run_date=start_date)


#scheduled rbrew job to repeat
def add_brew_job(day: int, hour: str, minute: str, duration: int):
    sched.add_job(trigger_brew, 'cron', [duration], day_of_week= day, hour=hour)

def trigger_brew(duration: int):
    print("trigger brew scheduler")


