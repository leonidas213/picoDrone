import datetime
import zlib
import socket
import base64
import numpy as np
import cv2
import os
import struct
import requests
import json
import time
import pprint


class Networking:

    @staticmethod
    def SendMessageTCP(Data, IP='', Port=0, compress=False, receive=True, PacketSize=65536):
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # soketi oluştur ve client olarak ata

        client.connect((IP, Port))  # soketi bağla


        if compress:

            client.send(zlib.compress(Data.encode("utf-8")))  # mesajı sıkıştır gönder
        else:
            client.send(Data)  # mesajı sıkıştırmadan gönder
            # mesaj var ise al
        if receive:
            a = client.recv(PacketSize)
            return a
        return ""

    @staticmethod
    def SendMessageUDP(Data="", IP='', Port=0, compress=False, receive=True):
        client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # soketi oluştur ve client olarak ata
        if compress:
            client.sendto(zlib.compress(Data.encode("utf-8")), (IP, Port))  # mesajı sıkıştır gönder
        else:
            client.sendto(Data.encode("utf-8"), (IP, Port))  # mesajı sıkıştırmadan gönder
        if receive:
            a = client.recvfrom(65536)  # mesaj var ise al
            mess, addr = a
            return mess.decode("utf-8"), addr
        return ""



"""class VideoSenderTCP:
    def __init__(self, ip: str = "127.0.0.1", port:int=5555):
        self.sender = imagezmq.ImageSender(connect_to="tcp://{}:{}".format(
            ip, port))

    def SendVideo(self, frame):
        try:
            frame = frame.get()
            Name = socket.gethostname()
            self.sender.send_image(Name, frame)
        except:
            pass"""
class VideoSenderUdp:

    def __init__(self,ip:str="127.0.0.1",port:int=25565,bufferSize:int=65536):

        self.buffer_size = bufferSize
        self.host_ip =ip  ## bunu user inputla mi yapacagiz bilmiyorum ondan dokunmadim
        self.port = port ## bu da ayni sekil
        self.set_socket()
    def set_socket(self):

        self.server_socket = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
        self.server_socket.settimeout(200)
        self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF,self.buffer_size)
        self.socket_addr = (self.host_ip,self.port)
        self.host_name= socket.gethostname()

        print("Sending To:",self.socket_addr)

    def get_socket(self):
        print("host ip-> "+self.host_ip+"\nport-> "+str(self.port)+"\nhost name-> "+self.host_name)


    def get_message(self):
        self.msg, self.client_addr = self.server_socket.recvfrom(self.buffer_size)
        print("recieved connection from -> ",self.client_addr)
        return self.msg, self.client_addr

    def send_message(self,data,DataType:str):
        encoded, buffer = cv2.imencode('.jpg', data, [cv2.IMWRITE_JPEG_QUALITY, 100])
        lowres = 2
        while len(buffer) > 65000:
            encoded, buffer = cv2.imencode('.jpg', data, [cv2.IMWRITE_JPEG_QUALITY, 100 - 10 * lowres])
            lowres += 1
            if (lowres >= 9):
                break

        message = base64.b64encode(buffer)
        message=message.decode("utf-8")
        message=zlib.compress((DataType+message).encode("utf-8"))
        #print(len(message))

        try:
            self.server_socket.sendto(message, (self.host_ip, self.port))
        except Exception as e:
            pass
            #print("expt",e)#göndermeye çalıltıpı resim çok hüyük 65500 bişi bytrı geçiyor
            #print(len(data))#sendvideoyu kapatırsandüzelir bak kalmadı
class VideoReceiverUdp:
    def __init__(self,ip:str="",port:int=25565):
        self.serv=ServerUDP(ip,port)
    def Read(self):
        msg,addr= self.serv.ListenRecv(False,True)
        decrypted = base64.b64decode(msg)
        decrypted = np.frombuffer(decrypted, np.uint8)
        return cv2.imdecode(decrypted, 1)

class ServerTCP:
    def __init__(self, ip: str, port: int):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.bind((ip, port))


    def Listen(self):  # serverı dinlemeye al
        self.socket.listen()
        conn, addr = self.socket.accept()
        return conn, addr
    def ListenReceive(self,length=65536, compress=False):  # serverı dinlemeye al
        self.socket.listen()
        conn, addr = self.socket.accept()
        return self.ReceiveMessage(conn,length,compress)


    @staticmethod
    def ReceiveMessage(conn, length=65536, compress=False):  # mesajı al
        if compress:
            return zlib.decompress(conn.recv(length)).decode("utf-8")
        else:
            return conn.recv(length).decode("utf-8")

    @staticmethod
    def SendMessage(message: str, conn):  # mesaj gönder
        conn.send(message.encode("utf-8"))


class ServerUDP:

    def __init__(self, ip: str, port: int):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.socket.bind((ip, port))

    def ListenRecv(self,decode=True,decompress=False):  # serverı dinlemeye al ve mesaj al
        packet = self.socket.recvfrom(65536)
        if decode:
            if decompress:
                msg= zlib.decompress(packet[0]).decode("utf-8")
            else:
                msg = packet[0].decode("utf-8")

        else:
            if decompress:
                msg= zlib.decompress(packet[0])
            else:
                msg=packet[0]
        addr = packet[1]
        return msg, addr

    def SendMessage(self, message: str, addr):  # mesaj gönder
        self.socket.sendto(message.encode("utf-8"), addr)


class Pipeline:

    def __init__(self, PipeName: str, Create=False):
        if Create:
            os.mkfifo(r'\\.\pipe/' + PipeName)
        self.PipeName = PipeName
        self.f = open(r'\\.\pipe/' + PipeName, 'r+b', 0)

    def Close(self):
        os.unlink(r'\\.\pipe/' + self.PipeName)

    def WriteRead(self, msg: str, CompressWrite=False, CompressRead=False):
        self.Write(msg, CompressWrite)
        return self.Read(CompressRead)

    def Write(self, msg: str, compress=False):
        if compress:
            msg = zlib.compress(msg.encode("utf-8"))
        else:
            msg = msg.encode("utf-8")
        self.f.write(struct.pack('I', len(msg)) + msg)  # Write str length and str
        self.f.seek(0)  # EDIT: This is also necessary

    def Read(self, compress=False) -> str:
        Packet = struct.unpack('I', self.f.read(4))[0]  # Read str length
        incomingMessage = self.f.read(Packet)
        self.f.seek(0)  # Important!!!
        if compress:
            incomingMessage = zlib.decompress(incomingMessage)
        else:
            incomingMessage = incomingMessage.decode('ascii')  # Read str
        return incomingMessage

class API:
    BUFFER_SIZE = 65536

    TeknoIP =""
    def __init__(self,IP,komut,jsonData):#"adress",("kullanıcıadı","şifre")
        self.TeknoIP=IP
        self.session=requests.Session()
        self.session_cookie = ""
        self.create_session(komut,jsonData)

    def status_identf(self,status_code):

        if status_code == 200:

            return "\nIslem basarili status = 200"

        elif status_code == 400:

            return  "\nBad request status = 400, contenti incele"

        elif status_code == 401 :

            return  "\nYetkisiz erisim status = 401, server baglantisini , bilgileri ve cookieleri kontrol et"

        elif status_code == 403:

            return "\nYasakli erisim status = 403, admin paneline erismeye calisiyor labilirsin"

        elif status_code == 404:

            return "\nUrl hatali yada endpoint hatali"

        elif status_code == 405 :

            print("\nMethodun yanlis (get post)")

        elif status_code == 500:

            print("\nServerde sikinti var. Server side problemi")


    def create_session(self,komut, jsonData):

        if self.session_cookie == "":
            try:
                jsonData = json.dumps(jsonData)
                jsonData = json.loads(jsonData)

                response = self.session.post(self.TeknoIP + komut, json=jsonData )

                if response is not None:
                    print(self.status_identf(response.status_code)+" "+response.content.decode())
                    if response.status_code == 200 :
                        self.session_cookie = response.cookies

                    else :
                        print("Session yaratilamadi ")
                else:
                    print("Bad request")

            except Exception as e :
                print(e)

        else :
            print("Zaten session açik")
    @staticmethod
    def fileWrite(Data, FileName, FileLocation):
        with open(FileLocation + FileName, "wb") as write_file:
            write_file.write(Data)


    def ResponseGet(self,komut):
        if self.session_cookie != "" :
            try:
                response = self.session.get(self.TeknoIP + komut,headers={"X-CSRFToken":self.session.cookies.get("csrftoken")})

                print(self.status_identf(response.status_code)+" " +"response content -> "+str(response.content.decode("utf-8")))
                """
                proof of work icin
                self.fileWrite(response.content,"temp.jpg","/home/ras/DroneTakip/LibsAndClasses/")
                time.sleep(1)
                dat=cv2.imread("temp.jpg")
                cv2.imshow("denem",dat)
                """

            except Exception as e:
                print(e)

    def ResponsePut(self,komut,jsonData,isimage):
        if self.session_cookie != "":
            try:
                if not isimage:
                    jsonData = json.dumps(jsonData)
                    jsonData = json.loads(jsonData)

                    response = self.session.put(self.TeknoIP + komut, json=jsonData,cookies=self.session_cookie,headers={"X-CSRFToken":self.session.cookies.get("csrftoken")})

                else :
                    response = self.session.put(self.TeknoIP + komut, data=jsonData,cookies=self.session_cookie,headers={"X-CSRFToken":self.session.cookies.get("csrftoken")})

                if response is not None:
                    #print("response -> " + str(response.content) + " status code -> " + self.status_identf(response.status_code))
                    if response.status_code == 200:
                        print("Islem basarili  status code -> 200")

                else:
                    print("Bad request")

            except Exception as e:
                print(e)

    def ResponsePost(self,komut, jsonData):
        response= "{}"
        if self.session_cookie != "" :
            try:
                jsonData = json.dumps(jsonData)
                jsonData = json.loads(jsonData)
                response = self.session.post(self.TeknoIP + komut, json=jsonData)
                if response is not None:
                    print("response -> " + str(response.content.decode("utf-8")) + " status code -> " + self.status_identf(response.status_code))
                    #print(self.session.cookies.get("csrftoken"))
                    pass

                else:
                    print("Bad request")

            except Exception as e :
                print(e)

        else :
            print("Cookie olmadigi icin post atilamadi")
        return response.content.decode("utf-8"), response.status_code

    def get_teams(self):
        komut = "/api/teams"
        if self.session_cookie != "":
            try:
                response = self.session.get(self.TeknoIP + komut)
                pprint.pprint(response.json(),width=1)
            except Exception as e :
                print(e)

        else :
            print("Cookie olmadigi  icin takimlar cekilemedi")

    def get_mission(self, id:str):
        komut = "/api/missions/"+"/"+id
        if self.session_cookie !=  "":
            try:
                response = self.session.get(self.TeknoIP + komut)
                pprint.pprint(response.json(), width=1)

            except Exception as e :
                print(e)

        else :
            print("Cookie olmadigi icin gorevler cekilemedi")

    def send_telem(self,telem):
        """
        SUAS SUNUCUSU SADECE  ILK 4 HEADERI ISTIYOR ONDAN DOLAYI BU ILERIDE AKTIF EDILECEKTIR
        telem = {
            "latitude": 38,
            "longitude": -75,
            "altitude": 50,
            "heading": 90,
            "takim_numarasi": 41153,
            "IHA_enlem": 40.22796,
            "IHA_boylam": 28.99716,
            "IHA_irtifa": 50,
            "IHA_dikilme": 5,
            "IHA_yonelme": 256,
            "IHA_yatis": 0,
            "IHA_hiz": 223,
            "IHA_batarya": 20,
            "IHA_otonom": False,
            "IHA_kilitlenme": False,
            "Hedef_merkez_X": 315,
            "Hedef_merkez_Y": 220,
            "Hedef_genislik": 12,
            "Hedef_yukseklik": 46,
            "GPSSaati": {"saat": 19, "dakika": 1, "saniye": 23, "milisaniye": 507}
        }
        """

        telem = json.dumps(telem)
        telem = json.loads(telem)


        self.ResponsePost("/api/telemetry",telem)

    def send_vid(self):

        data =  {}
        with open ("denem.jpeg", 'rb') as f:
            img  = f.read()


        self.ResponsePut("/api/maps/1/testuser",img,True)



if __name__ == "__main__":
    """
      credentials={
        "username": "testuser",
        "password": "testpass"
    }
    telem ={
        "latitude": 38,
        "longitude": -75,
        "altitude": 50,
        "heading": 5
    }
    Data = {"takim_numarasi": 1234, "IHA_enlem": 47.8,
            "IHA_boylam": 28.7,
            "IHA_irtifa": 50,
            "IHA_dikilme": 0,
            "IHA_yonelme": 20.5, "IHA_yatis": 30,
            "IHA_hiz": 5, "IHA_batarya": 80,
            "otonom": int(1),
            "kilitlenme": int(0), "Hedef_merkez_X": 0,
            "Hedef_merkez_Y": 0,
            "Hedef_genislik": 0,
            "Hedef_yukseklik": 0,
            }
    odlcs = {
      "id": 1,
      "mission": 1,
      "type": "STANDARD",
      "latitude": 38,
      "longitude": -76,
      "orientation": "N",
      "shape": "RECTANGLE",
      "shapeColor": "RED",
      "autonomous": False
    }
    
    api.get_teams()
    api.get_mission("1")


    #api.send_vid()
    #api.ResponseGet("/api/maps/1/testuser",True)""


    api.send_telem(Data)

    credentials = {
        "username": "testadmin",
        "password": "testpass"
    }

    """


    KilitDen = {
        "kilitlenmeBaslangicZamani": {
            "saat": 19,
            "dakika": 1,
            "saniye": 23,
            "milisaniye": 507
        },
        "kilitlenmeBitisZamani": {
            "saat": 19,
            "dakika": 1,
            "saniye": 45,
            "milisaniye": 236
        },
    }
    KamikazeDen = {
        "kamikazeBaslangicZamani": {
            "saat": 19,
            "dakika": 1,
            "saniye": 23,
            "milisaniye": 507
        },
        "kamikazeBitisZamani": {
            "saat": 19,
            "dakika": 1,
            "saniye": 28,
            "milisaniye": 236
        },
        "qrMetni": "teknofest2022"
    }
    info = {
        "kadi":"ikukirmizikanatlar",
        "sifre":"sa9g1x8bt3"
    }
    #print(json.dumps(telem))
    api = API("http://10.0.0.15:64559/","api/giris/",info)
    api.ResponseGet("api/qr_koordinati")

    #api.ResponsePost("api/kamikaze_bilgisi/",KamikazeDen)
    #api.ResponsePost("api/kilitlenme_bilgisi/",KilitDen)
    num=0
    while 0:
        telem = {
            "Takim_numarasi": 8,
            "IHA_enlem": 41.22796,
            "IHA_boylam": 28.99716,
            "IHA_irtifa": 50,
            "IHA_dikilme": 5,
            "IHA_yonelme": 256,
            "IHA_yatis": 0,
            "IHA_hiz": 223,
            "IHA_batarya": 20,
            "IHA_otonom": 1,
            "IHA_kilitlenme": 0,
            "Hedef_merkez_X": 315,
            "Hedef_merkez_Y": 220,
            "Hedef_genislik": 12,
            "Hedef_yukseklik": 46,
            "GPSSaati": {"saat": datetime.datetime.now().hour, "dakika": datetime.datetime.now().minute,
                         "saniye": datetime.datetime.now().second,
                         "milisaniye": datetime.datetime.now().microsecond % 1000},
        }



        api.ResponsePost("api/telemetri_gonder/",telem)
        #api.ResponseGet("api/sunucusaati")
        time.sleep(2)

    #api.ResponseGet("api/kamikaze_gorevi/",0)



