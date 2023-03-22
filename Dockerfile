FROM python:3.9

WORKDIR /code

COPY ./requirements.txt /code/requirements.txt

RUN pip install --no-cache-dir --upgrade -r requirements.txt
RUN pip install uvicorn[standard]

COPY . /code/app

ENV PYTHONPATH="/code/app"

CMD ["uvicorn", "app.main:app", "--host", "0.0.0.0", "--port", "80"]
# CMD ["python", "app/run.py"]

EXPOSE 80
