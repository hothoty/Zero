using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SampleServer2
{
    public class Async
    {
        /// <summary>
        /// Async/Await을 원하는 곳에서 바로 사용하기 위한 함수
        /// </summary>
        /// <param name="async_func"></param>
        /// <param name="action_result"></param>
        public static async void Run(Action async_func, Action action_result)
        {
            await Task.Run(() =>
            {
                async_func();
            });

            action_result();
        }
    }
}
