FROM python:3.8-slim

WORKDIR /app

COPY . .

RUN pip install requirements.txt

ENV FLASK_APP=login.py

CMD ["flask", "run", "--host", "0.0.0.0"]