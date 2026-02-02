import socket
import threading as th
from PyChess import desk 
import time # \tst
import re



# Глобальные настройки сделаны на скорую руку
class settings:

    # Настройки сети
    socket_addres = "localhost"
    socket_port = 4567
    max_idle_socket_connections = 10
    bytes_recv_portion = 1024

    # Настройки менеджера пользователей 
    max_idle_users = 1000
    max_playing_users = 1000
    is_max_rejectons_del_on = True
    max_party_rejections = 100
    
    

    

class log():
    
    def __init__(self, pritable = False):
        self.log_msg = []
        self.lock = th.Lock()
        self.pritable = pritable

    def __call__(self, *args):
        # Логгирование вызывается в потоке, поэтому замок не даёт иному потоку записать лог
        self.lock.acquire()
        self.log_msg.append(f"[{th.current_thread().name}] {str(*args)}")
        if self.pritable:
            print()
            print(str(*args))
        self.lock.release()


    def save(self):
        with open("server.log", "w") as file:
            file.write("\n".join(self.log_msg))

    @property
    def msg(self):
        return "\n".join(self.log_msg)

log = log(pritable = True)

# Каждый из потоков находится в состоянии ожидания
# Отвечает за работу сети, общение с пользователем

"""
Для потока интерфейса приорететными запросами считаются запросы от потока обработки, затем от пользователей

Новые соединения образуються только после окончания обрабоки текущего соединения..

Поток интерфейса ожидает входящих подключений, затем проводит их обработку, а затем сообщает потоку обработки о необходимости обработать данные, затем ждёт ответ и передаёт его клиенту.

По сути, клиент каждый раз требует соврешение какого-либо действия:

 1) Создание партии
 2) Совершение хода

Клиент со стороны сервера имеет состояние ожидания партии, или состояние игры. 

Протокол состоит из набора текстовых сообщений в кодировке UTF-8:
    Сообщения клиента:
    NEW {id} - сообщение о желании сыграть. Сервер возвращает SUCCSES или FAILLURE, соответственно добавляет клиента в очередь
    MOVE x y - совершить ход из позиции x в позицию y. S/F возврат
    ACCEPT - ответ на принятие запроса партии -- когда появляется партнёр для партии, сервер должен получить это в ответ.

    Сообщения сервера:
    SUCCSES - собщение об успехе операции
    FAILLURE - сообщение о неудаче
    ERROR - ошибка 
    REQUEST {req_id} -- запрос на создание партии ожидающему клиенту. 
    REJECT - отказ в ожидании партии - если клиент слишком долго ждал или слишком много отказывался.
"""

# Режим простоя потока включается лишь тогда, когда работы нет
# За очередь игроков отвечает именно этот класc
class PlayerManager:
    def __init__(self):
        self.waiting_clients = []
        self.partys = []
        self.lock = th.Lock()

    
    def add_waiting_client(self, str_id, addres):
        if len(self.waiting_clients) <= settings.max_idle_users:
            self.waiting_clients.append([str_id, addres])
            if len(self.waiting_clients) >= 2:
                partyUpdateEvent.set()
            return True
        return False


class EventJobsStack:

    def __init__(self, func_dict):
        self.func_dict = func_dict 
        self.condition = th.Condition()
        






class NetWorker(th.Thread):
    name = "networker"
    def run(self):
        #nonlocal plMngr # Глобальный для обоих потоков
        log(f"Create socket with addres {settings.socket_addres} on port {settings.socket_port}")
        self.sock = socket.create_server((settings.socket_addres, settings.socket_port))
        self.sock.listen(settings.max_idle_socket_connections)

        while True:
            str_msg, addres = self.wait_for_socket_msg() 
            answer = self.proceed_request(str_msg, addres)
            log(f"Answering {answer}")
            answer = "ERROR"
            self.sock.sendall(answer.encode())

    # Обрабатывает запрос от клиента
    def proceed_request(self, message, addres):
        message = message.strip()

        create_new_expectation = re.match(r"NEW\s\w+", message)
        if create_new_expectation:
            is_client_add = plMngr.add_waiting_client(create_new_expectation.string.split()[1], addres)


    # Ожидает подключения клинета к серверу, получает данные от клиента и возвращает их в виде строки 
    def wait_for_socket_msg(self):
        connection, addres = self.sock.accept()
        log(f"Client {addres} connected")
        msg = ""
        data = connection.recv(settings.bytes_recv_portion)
        while data:
            msg += data.decode()
            data = connection.recv(settings.bytes_recv_portion)
        log(f"Client {addres} send {msg}")
        return msg, addres


# Отвечает за проверку хода, статус партии
# Второй поток может попросить обновить партию или создать новую
class PartyManager(th.Thread):
    name = "partyManager"

    def create_party():
        pass

    def update_party():
        pass

    def run(self):
        #nonlocal plMngr
        pass


# Метод старта
def Main():
    try:
        # Опиция daemon отвечает за запуск потоков в фоновом режиме. Программа завершиться при завершении главного потока
        netWrk = NetWorker(daemon = True)
        prtMngr = PartyManager(daemon = True)
        plMngr = PlayerManager()

        
        # Событие, за которыми следит partyManager
        partyManagerEvent = th.Event()
        
        # События, за которыми следит NetWorker
        partyEndEvent = th.Event()

        # Запуск потоков 
        prtMngr.start()
        netWrk.start()

        # В основном потоке реализованна интерактивная консоль управления 
        is_server_run = True
        print("Chess server started. Type \"shutdown\" or Ctrl-C to shutdown server")
        while is_server_run:
            msg = input(">>> ")
            msg = msg.strip()
            if msg == "shutdown":
                is_server_run = False


    except KeyboardInterrupt:
        pass
    finally:
        print("Log:")
        print(log.msg)
        log.save()
        

def test():
    dsk = desk()


if __name__ == "__main__":
    Main()
