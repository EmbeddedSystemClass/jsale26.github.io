import binascii
from socket import *
from tkinter import *

def extract_mx(response, name, serverAddr, cname):
    
    if (response[12:16] == "0000"):
        return "No mail servers."
    
    mxStr = ""
    responses = response.split("c00c")

    for j in range(1, len(responses)):
        mxBuff = ""
        i = 26
        while ((responses[j][i:(i + 2)] != "00")
                   and (responses[j][i:(i + 2)] != "c0")
                   and (responses[j][i:(i + 2)] != "")):
            
            if ((96 < int(responses[j][i:(i + 2)], 16) < 123) or
                (32 < int(responses[j][i:(i + 2)], 16) < 91)):
                mxBuff = mxBuff + chr(int(responses[j][i:(i + 2)], 16))
                
            else:
                mxBuff = mxBuff + "."
    
            i = i + 2

        if (responses[j][i:(i + 2)] == "c0"):
            pointer = 2 * int(responses[j][(i + 2):(i + 4)], 16)
            while ((response[pointer:(pointer + 2)] != "00")
                   and (response[pointer:(pointer + 2)] != "c0")):
                if ((96 < int(response[pointer:(pointer + 2)], 16) < 123) or
                    (32 < int(response[pointer:(pointer + 2)], 16) < 91)):
                    mxBuff = (mxBuff 
                            + chr(int(response[pointer:(pointer + 2)], 16)))
                    
                else:
                    mxBuff = mxBuff + "."

                pointer = pointer + 2

        question = generate_question(mxBuff, "00 01", "forward")
        ipv4 = extract_ip(handle_query(question, serverAddr), 4)
        
        mxStr = mxStr + mxBuff + "    :    " + ipv4 + "\n"
    
    if (cname != "Not Available."):
        if (mxStr.count("\n") == 1):
            return "No mail servers."
    
    return mxStr

def extract_host(response):

    if (response[12:16] == "0000"):
        return "Server can't find."
    
    host = ""

    i = 112
    
    while (i < len(response)):
        if ((96 < int(response[i:(i + 2)], 16) < 123) or
            (32 < int(response[i:(i + 2)], 16) < 91)):
            host = host + chr(int(response[i:(i + 2)], 16))
        else:
            host = host + "."

        i = i + 2

    for i in host:
        if (i == '.'):
            host = host[1:]
        else:
            break

    return host[:-1]

def extract_host1(response):
    
    """ Find arpa. Host name afterward """

    if (response[12:16] == "0000"):
        return "Server can't find."
    
    index = (response.index("61727061") - 14) * 2

    host_response = response[index:]
    
    i = 2
    host = ""
    while (i < len(host_response)):
        if (int(host_response[(i-2):i], 16) < 30):
            host = host + "."
        else:
            host = host + str(chr(int(host_response[(i - 2):i], 16)))
        i = i + 2
    
    for i in host:
        if (i == '.'):
            host = host[1:]
        else:
            break

    j = 0
    while (j < len(host)):
        if not ((96 < ord(host[j]) < 123) or
                (32 < ord(host[j]) < 91)):
            if not ((96 < ord(host[j]) < 123) or
                (32 < ord(host[j]) < 91)):
                host = host[:(j - 1)]
                break
        j = j + 1

    return host 

def extract_cname(response, name):
    
    if (response[12:16] == "0000"):
        return "Not available."

    response = response[(response.index("0005") + 32):]

    cname = ""
    i = 0
    while ((response[i:(i + 2)] != "00") and (response[i:(i + 2)] != "c0")):
        if ((96 < int(response[i:(i + 2)], 16) < 123) or
            (32 < int(response[i:(i + 2)], 16) < 91)):
            cname = cname + chr(int(response[i:(i + 2)], 16))
             
        else:
             cname = cname + "."
    
        i = i + 2

    if (response[i:(i + 2)] == "00"):
        return cname[1:]

    else:
        cname = cname + name[name.index("."):]
        return cname[1:]   
    
def extract_ip(response, ipv):
    if (ipv == 6):
        maxNum = 32
        const = 4
    else:
        maxNum = 8
        const = 2
        
    ip_response = response[-maxNum:]

    ip = ""
    
    i = const
    while (i <= maxNum):
        if (ipv == 6):
            ip = ip + ip_response[(i - const):i].upper()
        else:
            ip = ip + str(int(ip_response[(i - const):i], 16))

        if (i != maxNum):
            if (ipv == 6):
                ip = ip + ":"
            else:
                ip = ip + "."

        i = i + const

    return ip


def send_message1(message, addr, port):
    
    s = socket(AF_INET, SOCK_DGRAM

    print(binascii.unhexlify(message))
    message = message.translate(None, '\x')
    print(message)
    
               

    s.sendto(binascii.unhexlify(message), (addr, port))

    """ Coming back down socket """
    
    data, _ = s.recvfrom(4096)

    """ Close the socket """
    
    s.close()

    return binascii.hexlify(data).decode("utf-8")

def send_message(message, addr, port):
    
    server_info = (addr, port)

    sock = socket(AF_INET, SOCK_DGRAM)
        
    sock.sendto(binascii.unhexlify(message), server_info)
    data, _ = sock.recvfrom(4096)
    
    sock.close()
    return binascii.hexlify(data).decode("utf-8")


def handle_query(question, dnsIP):
    message = header + " " + question
    message = message.replace(" ", "").replace("\n", "")
    response = send_message(message, dnsIP, 53)
    return response

def generate_question(userInput, qtype, direction):

    periodCount = userInput.count('.') + 1
    inputList = userInput.split('.');

    question = ""

    for i in range(0, periodCount):
        
        if (direction == "forward"):
            length = len(inputList[i])
        elif (direction == "reverse"):
            length = len(inputList[periodCount - 1 - i])
        

        """ Determine number of bytes """
        
        if (length < 10):
            question = question + "0" + str(length) + " "
        else:
            hexStr = hex(len(inputList[i])).upper()
            hexStr = hexStr[2:]
            if (length < 16):
                question = question + "0" + hexStr + " "
            else:
                question = question + hexStr + " "

        """ Determine series of bytes to follow """

        if (direction == "forward"):
            for j in range(0, length):
                charVal = hex(ord(inputList[i][j])).upper()
                charVal = charVal[2:]
                question = question + charVal + " "
                
        elif (direction == "reverse"):
            for j in range(0, length):
                decVal = str(int(inputList[periodCount - 1 - i][j]) + 30)
                question = question + decVal + " "

    """ in-addr.arpa for reverse """

    if (direction == "reverse"):
        question = question + "07 69 6E 2D 61 64 64 72 04 61 72 70 61"
        
    """ End byte padding """

    question = question + "00"


    """ Qtype: A - IPv4, AAAA (28 == 1C) - IPv6 """

    question = question + qtype

    """ Finally, Qclass """

    question = question + "00 01"
    
    return question

def validIPv4(address):

    for item in address:
        if item.isspace():
            return False
    
    parts = address.split(".")

    if (len(parts) != 4):
        return False
    
    for item in parts:
        
        try:
            int(item)
        except ValueError:
            return False
        
        if not (0 <= int(item) <= 255):
            return False
        
    return True

def dns_lookup():
    serverAddr = e1.get()
    name = e2.get()
    e3.configure(bg = "white")
    flag = 0

    if (validIPv4(serverAddr) == True):
        e1.configure(bg = "green")
    else:
        e1.configure(bg = "red")
        flag = 1
    
    if (name != ""):
        e2.configure(bg = "green")
    else:
        e2.configure(bg = "red")
        flag = 1

    for item in name:
        if item.isspace():
            e2.configure(bg = "red")
            flag = 1

    if (flag == 1):
        return
    
    name_label.config(text = name)
    
    question = generate_question(name, "00 01", "forward")
    ipv4 = extract_ip(handle_query(question, serverAddr), 4)
    ipv4_label.config(text = ipv4)

    question = generate_question(name, "00 1C", "forward")
    ipv6 = extract_ip(handle_query(question, serverAddr), 6)
    ipv6_label.config(text = ipv6)
    
    question = generate_question(name, "00 05", "forward")
    cname = extract_cname(handle_query(question, serverAddr), name)
    cname_label.config(text = cname)

    question = generate_question(name, "00 0F", "forward")
    mx = extract_mx(handle_query(question, serverAddr), name,
                        serverAddr, cname)
    mx_label.config(text = mx)

    
def rev_dns_lookup():
    serverAddr = e1.get()
    e2.configure(bg = "white")
    ipv4 = e3.get()
    flag = 0

    if (validIPv4(serverAddr) == True):
        e1.configure(bg = "green")
    else:
        e1.configure(bg = "red")
        flag = 1

    if (validIPv4(ipv4) == True):
        e3.configure(bg = "green")
    else:
        e3.configure(bg = "red")
        flag = 1

    if (flag == 1):
        return

    question = generate_question(ipv4, "00 0C", "reverse")
    host = extract_host(handle_query(question, serverAddr))


    if (host != "Server can't find."):
            
        name_label.config(text = host)
        
        question = generate_question(host, "00 01", "forward")
        ipv4 = extract_ip(handle_query(question, serverAddr), 4)
        ipv4_label.config(text = ipv4)

        question = generate_question(host, "00 1C", "forward")
        ipv6 = extract_ip(handle_query(question, serverAddr), 6)
        ipv6_label.config(text = ipv6)

        question = generate_question(host, "00 05", "forward")
        cname = extract_cname(handle_query(question, serverAddr), host)
        cname_label.config(text = cname)

        question = generate_question(host, "00 0F", "forward")
        mx = extract_mx(handle_query(question, serverAddr), host,
                        serverAddr, cname)
        mx_label.config(text = mx)
        
    else:
        name_label.config(text = "No hostname available on this server.")
        ipv4_label.config(text = e3.get())
        ipv6_label.config(text = "Unable to retrieve.")
        cname_label.config(text = "Unable to retrieve.")
        mx_label.config(text = "Unable to retrieve.")
        
def set_ipv4():
    ipv4 = ipv4_label.cget("text")
    e3.delete(0, END)
    e3.insert(0, ipv4)

def set_name():
    name = name_label.cget("text")
    e2.delete(0, END)
    e2.insert(0, name)





""" Main program - GUI initialisation """

header = "AA AA 01 00 00 01 00 00 00 00 00 00"

master = Tk()
master.title("DNS Lookup - Joshua Salecich (s43981722) - COMS3200")
    
Label(master, text="Server Address:").grid(row = 0, column = 0)
Label(master, text="Domain/Host Name:").grid(row = 1, column = 0)
Label(master, text="IPv4 Address:").grid(row = 2, column = 0)

Label(master, text="Domain/Host name:").grid(row = 6, column = 0)
Label(master, text="IPv4 Address:").grid(row = 7, column = 0)
Label(master, text="IPv6 Address:").grid(row = 8, column = 0)
Label(master, text="Canonical Host Name:").grid(row = 9, column = 0)
Label(master, text="Mail Server(s):").grid(row = 10, column = 0)

name_label = Label(master)
name_label.grid(row = 6, column = 1)

ipv4_label = Label(master)
ipv4_label.grid(row = 7, column = 1)

ipv6_label = Label(master)
ipv6_label.grid(row = 8, column = 1)

cname_label = Label(master)
cname_label.grid(row = 9, column = 1)

mx_label = Label(master)
mx_label.grid(row = 10, column = 1)

e1 = Entry(master)
e2 = Entry(master)
e3 = Entry(master)

e1.grid(row=0, column=1)
e1.insert(0, "8.8.8.8")
e2.grid(row=1, column=1)
e3.grid(row=2, column=1)

dnsLookup = Button(master, text='DNS Lookup', command = dns_lookup)
dnsLookup.grid(row = 3, column = 0)
revLookup = Button(master, text='Reverse DNS Lookup', command = rev_dns_lookup)
revLookup.grid(row = 3, column = 1)
setIPv4 = Button(master, text='Set IPv4', command = set_ipv4)
setIPv4.grid(row = 7, column = 2)
setName = Button(master, text='Set Name', command = set_name)
setName.grid(row = 6, column = 2)
                 
    
mainloop()

