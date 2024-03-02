from NetworkClass import Networking

if __name__ == "__main__":
    channel1 = 1500
    channel2 = 1500
    channel3 = 1000
    channel4 = 1500
    channel5 = 1500
    channel6 = 1500

    dat = "chp"
    dat = bytearray(dat, 'utf-8')
    dat.append((channel1>>8)&0xFF)
    dat.append(channel1&0xFF)
    dat.append((channel2>>8)&0xFF)
    dat.append(channel2&0xFF)
    dat.append((channel3>>8)&0xFF)
    dat.append(channel3&0xFF)
    dat.append((channel4>>8)&0xFF)
    dat.append(channel4&0xFF)
    dat.append((channel5>>8)&0xFF)
    dat.append(channel5&0xFF)
    dat.append((channel6>>8)&0xFF)
    dat.append(channel6&0xFF)

    print(dat)
    a = Networking.SendMessageTCP(dat, "192.168.0.11", 4242, receive=True)
    print(a)

