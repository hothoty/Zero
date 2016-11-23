using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace SampleServer2
{
    public partial class Form1 : Form
    {
        Server svr;

        Int32 count_listbox = 1;

        DateTime lasttime_update_gui;


        public Form1()
        {
            InitializeComponent();
        }

        public void Put(string txt, params object[] args)
        {
            Put(string.Format(txt, args));
        }
        public void Put(string txt)
        {
            this.Invoke(new Action(() =>
            {
                listBox1.Items.Add(string.Format("{0}:  {1}", count_listbox++, txt));

                if (listBox1.Items.Count > 1000)
                    listBox1.Items.RemoveAt(0);

                listBox1.SetSelected(listBox1.Items.Count - 1, true);
            }));
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            svr = new Server(this);

            lasttime_update_gui = DateTime.Now;

            svr.Start();

            timer1.Start();
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            svr.Update();


            if (DateTime.Now.Subtract(lasttime_update_gui).TotalMilliseconds > 500)
            {
                lasttime_update_gui = DateTime.Now;

                label_user.Text = string.Format("user connected : {0}", svr.RemoteClients.Count);
            }
        }
    }
}
