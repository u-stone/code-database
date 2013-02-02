using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.IO;
using System.Threading;

namespace Office2Pdf
{
    /// <summary>
    /// 该类负责将office文件转化类转化好的文件全路径发送给指定的URL页面处理
    /// </summary>
    class PostHttpMsg
    {
        private string m_Url = "http://dl.hao2580.com/tpdf/php/services/view2.php";//"http://www.contoso.com/example.aspx"
        private string m_Data = "";
        private ManualResetEvent allDone = new ManualResetEvent(false);
        public long PostMsg()
        {
            HttpWebRequest request = (HttpWebRequest)WebRequest.Create(m_Url);
            request.ContentType = "application/x-www-form-urlencoded";
            request.Method = "POST";
            //开始异步操作
            //开始对用来写入数据的System.IO.Stream 对象的异步请求。
            request.BeginGetRequestStream(new AsyncCallback(ReadCallback) , request);
            //维持主线程继续直到异步操作完成。
            allDone.WaitOne();
            //获取响应
            HttpWebResponse response = (HttpWebResponse)request.GetResponse();
            //获取响应流
            Stream streamResponse = response.GetResponseStream();
            //读取响应流
            StreamReader streamRead = new StreamReader(streamResponse);
            //读取响应流
            string responseString = streamRead.ReadToEnd();
            //Console.WriteLine(responseString);
            streamResponse.Close();
            streamResponse.Dispose();
            streamRead.Close();
            streamRead.Dispose();
            response.Close();
            //Console.ReadKey();
            return 0;
        }
        private void ReadCallback(IAsyncResult asynchronousResult)
        {
            HttpWebRequest request = (HttpWebRequest)asynchronousResult.AsyncState;
            //结束请求操作
            //结束对用于写入数据的System.IO.Stream 对象的异步请求。
            Stream postStream = request.EndGetRequestStream(asynchronousResult);
            string postData =  "doc=" + m_Data;
            //将字符串转化为字节数组
            byte[] byteArray = Encoding.UTF8.GetBytes(postData);
            //向请求流中写入字节
            postStream.Write(byteArray, 0, byteArray.Length);

            postStream.Close();
            postStream.Dispose();
            allDone.Set();
        }
        /// <summary>
        /// 传送的数据，这里就是文件的全路径
        /// </summary>
        public string DataPost
        {
            get { return m_Data; }
            set { m_Data = value; }
        }
    }
}
