from consts import PUMP_FLOW_RATE

DISPENSER_MAP = {
    "mild_small": 2,
    "mild_medium": 3,
    "mild_large": 4,
    "medium_small": 3,
    "medium_medium": 5,
    "medium_large": 6,
    "bold_small": 4,
    "bold_medium": 6,
    "bold_large": 8,
}

VOLUME_MAP = {
    "small": 300,
    "medium": 450,
    "large": 600,
}


def get_num_dispenses(strength: str | None, size: str | None):
    if strength is None or size is None:
        return None
    key = strength + "_" + size
    return DISPENSER_MAP[key]


def get_pump_time_ms(size: str | None):
    if size is None:
        return None
    key = size
    return int(VOLUME_MAP[key] / PUMP_FLOW_RATE)
