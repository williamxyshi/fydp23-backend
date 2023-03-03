from asyncio import QueueEmpty
from datetime import datetime, timedelta

from apscheduler.schedulers.blocking import BlockingScheduler
from sqlalchemy.orm import Session

sched: BlockingScheduler = BlockingScheduler()
sched.start()


#single scheduled
def add_single_brew_job(start_date: int):
    sched.add_job(trigger_brew, 'date', run_date=start_date)


#scheduled rbrew job to repeat
def add_brew_job(day: int, hour: str, minute: str):
    sched.add_job(trigger_brew, 'cron', day_of_week= day, hour=hour)

def trigger_brew():
    print("trigger brew scheduler")


