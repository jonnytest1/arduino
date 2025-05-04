import asyncio
from datetime import datetime
from typing import Coroutine, List, Union

from FileChangeEvent import FileChangeEvent
from asyncEventHandler import AsyncEventHandler, EventIterator
from watchdog.observers import Observer
from copylibs import copy_all
loop = asyncio.get_event_loop()
queue = asyncio.Queue(loop=loop)


observerList: List[Observer] = []


async def on_file_change(path: str):
    if ".history" in path or path.endswith(".tmp"):
        return
    print(str(datetime.now().isoformat()) + "filechange "+path)
    copy_all()


async def consume(queue: asyncio.Queue):
    async for event in EventIterator(queue):
        evt: FileChangeEvent = event
        # print("queue item"+evt.path)
        task = loop.create_task(on_file_change(evt.path))

        await task


def watch(queue: asyncio.Queue, loop: asyncio.BaseEventLoop):
    # Watch directory for changes
    handler = AsyncEventHandler(queue, loop)

    observer = Observer()
    observer.schedule(
        handler, "D:\\Jonathan\\Projects\\arduino\\platformio-shared-lib\\ClonedLibrary", recursive=True)
    observer.start()


futureList: List[Union[asyncio.Future, Coroutine]] = [
    consume(queue)
]

futureList.append(loop.run_in_executor(None, watch, queue, loop))

try:
    loop.run_until_complete(asyncio.gather(*futureList))
except KeyboardInterrupt:
    loop.stop()
    for observer in observerList:
        observer.stop()
        observer.join()
