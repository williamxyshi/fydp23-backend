from asyncio import QueueEmpty
from datetime import datetime, timedelta

from apscheduler.schedulers.blocking import BlockingScheduler
from sqlalchemy.orm import Session


class ColdBrewScheduler:
  """
  CLASS that wraps a scheduler

  handles all transformation related functionality for a single organization
  """
  org_id: str
  sched: BlockingScheduler

  def __init__(self, org_id: str):
    self.org_id = org_id
    self.sched = BlockingScheduler()
    self.start_scheduler()

  def sync_brews(self):
    print("TODO IMPLEMENT")

  def notify_brew(self):
    print("TODO IMPLEMENT")


  def add_transformation(self):
    self.sched.add_job()

  def start_scheduler(self):
    self.sched.start()

  def stop_scheduler(self):
    self.sched.shutdown()
