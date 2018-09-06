import socket
import http.client
import datetime
run_flag = 0
day_list = ('Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun')
month_list = (('Jan','31'), ('Feb', '28'), ('Mar', '31'), ('Apr', '30'),
             ('May', '31'), ('Jun', '30'), ('Jul', '30'), ('Aug', '31'),
             ('Sep', '30'), ('Oct', '31'), ('Nov', '30'), ('Dec', '31'))

def find_nth(string, substring, n):
    start = string.find(substring)
    while start >= 0 and n > 1:
        start = string.find(substring, start+len(substring))
        n -= 1
    return start

def convert_date(date):
    zone = date[-3:]
    if zone == 'AEST':
        return

    zone = 'AEST'
    ''' Assuming GMT '''
    
    hours_index = date.find(':')
    hours = date[(hours_index - 2):hours_index]
    time = date[hours_index:(hours_index+6)]
    day = date[:3]
    day_index = day_list.index(day)
    date_num_index = date.find(' ')
    date_num = date[(date_num_index + 1):(date_num_index + 3)]
    month_index = find_nth(date, ' ', 3)
    month = date[(month_index - 3):month_index]
    year_index = find_nth(date, ' ', 4)
    year = date[(year_index - 4):year_index]

    for i in range(0, 12):
        if month == month_list[i][0]:
            month_num = i
            break
    
    hours = int(hours)
    date_num = int(date_num)
    year = int(year)
    hours = hours + 10
    if hours > 23:
        if date_num == int(month_list[month_num][1]):
            if month_num < 11:
                month = month_list[month_num + 1][0]
            else:
                month = month_list[0][0]
                year = year + 1
            date_num = 1
        else:
            date_num = date_num + 1
        hours = hours - 24
        if day_index < 7:
            day = day_list[day_index + 1]
        else:
            day = day_list[0]

    return day+', '+str(date_num)+' '+month+' '+str(year)+' '+str(hours)+time+' '+zone+'\n'


while True:
    print('---------------------------------------------------------------')
    if run_flag == 0:       
        web_server = input("\nEnter a web address for analysis: ")
    else:
        run_flag = 0
    
    base_request = 'GET / HTTP/1.1\r\nHost: \r\n\r\n'
    http_flag = web_server[:7]
    if http_flag == 'http://':
        web_server = web_server[7:-1]
    resource_index = find_nth(web_server, '/', 1)
    fullstop_index = find_nth(web_server, '.', 2)
    
    if resource_index != -1:
        resource = web_server[(resource_index + 1):]
        host = web_server[:resource_index]
        if resource_index <= fullstop_index:
            resource = 0
    else:
        host = web_server
        resource = 0
    
    http_request = (base_request[:21] + host + base_request[21:])    
    
    if resource != 0:
        http_request = http_request[:5] + resource + http_request[5:]
    
    host = host.encode()
    http_request = http_request.encode()
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((host, 80))
    s.send(http_request)
    
    http_header = s.recv(10000)
    http_header = http_header.decode()
    reply_index = http_header.find('HTTP/1.1')
    reply_end = http_header.find('\n')
    reply_code = http_header[(reply_index + 9):(reply_index + 12)]
    reply_meaning = http_header[(reply_index + 13):(reply_end - 1)]
    
    p_date = 0
    date_index_start = http_header.find('Date:')
    date_index_end = http_header.find('GMT')
    if date_index_end < date_index_start:
        date_index_end = find_nth(http_header, 'GMT', 2)
    if date_index_end == -1:
        date_index_end = http_header.find('AEST')
    if date_index_end != -1:
        date = http_header[(date_index_start + 6):date_index_end]
        date = convert_date(date[:-2])
    else:
        p_date = 1;
        now = datetime.datetime.now()
        date = (str(day_list[now.weekday()]) + ', ' + str(now.day) + ' ' +
                month_list[now.month - 1][0] + ' ' + str(now.year) + ' ' +
                str(now.hour) + ':' + str(now.minute) + ':' + str(now.second) +
                ' AEST\n')

    
    server_ip = s.getpeername()[0]
    server_port = s.getpeername()[1]
    client_ip= s.getsockname()[0]
    client_port = s.getsockname()[1]

    print("\nHTTP Protocol Analyzer, Written by Joshua Salecich, 43981722\n")
    print("URL Requested:", web_server, "\n")
    print("IP Address, # Port of the server:", server_ip, ",", server_port,
          "\n")
    print("IP Address, # Port of this client:", client_ip, ",", client_port,
          "\n")
    print("Reply Code:", reply_code, "\n")
    print("Reply Code Meaning:", reply_meaning, "\n")
    if date_index_end != -1:
        print("Date:", date)
    if p_date == 1:
        print("Date:", date)
        
    reply_code = int(reply_code)
    if (reply_code == 301) or (reply_code == 302):
        loc_index = http_header.find('Location:')
        
        for i in range(0, 1000):
            nl_num = find_nth(http_header, '\n', i)
            if nl_num > loc_index:
                loc_index_end = nl_num
                break
        web_server = http_header[(loc_index + 10):(loc_index_end)]
        print('Redirecting...', web_server)
        start = web_server[:4]
        if start == 'http':
            start = web_server[:5]
            if start != 'https':
                run_flag = 1
            else:
                print("Redirection was to https. Failure!\n")



