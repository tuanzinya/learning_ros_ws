import threading
import requests

class Download:
    def download(self, url, callback_word_count):
        print(f"线程：{threading.get_ident()} 开始下载:{url}")
        response = requests.get(url)
        response.encoding = "utf-8"
        callback_word_count(url, response.text)
    
    def Start_Download(self, url, callback_word_count):
        # target:开启线程后执行的函数
        # args:传入给函数的参数
        th = threading.Thread(target = self.download, args=(url, callback_word_count))
        th.start()
    
def word_count(url, result):
    print(f"{url}:{len(result)}->{result[:5]}")


def main():
    download = Download()
    download.Start_Download('http://127.0.0.1:8000/novel1.txt', word_count)
    download.Start_Download('http://127.0.0.1:8000/novel2.txt', word_count)
    download.Start_Download('http://127.0.0.1:8000/novel3.txt', word_count)
