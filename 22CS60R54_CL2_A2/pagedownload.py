from urllib.request import Request, urlopen

req = Request('https://en.wikipedia.org/wiki/2022_FIFA_World_Cup', headers={'User-Agent': 'Mozilla/5.0'})

webpage = urlopen(req).read()
mydata = webpage.decode("utf8")
f = open('fifa_data.html', 'w', encoding="utf-8")
f.write(mydata)
f.close()