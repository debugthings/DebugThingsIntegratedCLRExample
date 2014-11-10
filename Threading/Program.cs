using System;
using System.Collections.Generic;
using System.Text;
using System.Diagnostics;

namespace Threading
{
    class Program
    {
        static void Main(string[] args)
        {
            GC.Collect();
            System.Diagnostics.Trace.WriteLine("Test");
            
        }
    }
}
