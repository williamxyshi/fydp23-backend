from fastapi import FastAPI
import scheduler

from state import State
from models import SingleBrew, ScheduledBrew

app = FastAPI()

state = State()


@app.post("/brew/schedule")
async def scheduled_brew(data: ScheduledBrew):
    for day in data.days:
        await scheduler.add_brew_job(
            day=day,
            ready_time=data.ready_time,
            duration=data.duration,
            strength=data.strength,
            size=data.size,
            state=state,
        )


@app.post("/brew/single")
async def single_brew(data: SingleBrew):
    await scheduler.add_single_brew_job(
        ready_timestamp=data.ready_timestamp,
        duration=data.duration,
        strength=data.strength,
        size=data.size,
        state=state,
    )


@app.delete("/brew")
async def delete_brew():
    scheduler.remove_all_jobs()


@app.get("/action")
async def get_action():
    return {
        "action": state.action,
        "num_dispenses": state.num_dispenses(),
        "pump_time": state.pump_time_ms(),
    }


@app.get("/start")
async def start():
    state.start_event.set()


@app.get("/finish")
async def finish():
    state.finish_event.set()
