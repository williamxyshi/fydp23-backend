from asyncio import QueueEmpty
from datetime import datetime, timedelta

from apscheduler.schedulers.blocking import BlockingScheduler
from sqlalchemy.orm import Session

sched: BlockingScheduler = BlockingScheduler()


def sync_brews():
    print("TODO IMPLEMENT")


def notify_brew():
    print("TODO IMPLEMENT")


def add_transformation():
    sched.add_job()


def start_scheduler():
    sched.start()


def stop_scheduler():
    sched.shutdown()
