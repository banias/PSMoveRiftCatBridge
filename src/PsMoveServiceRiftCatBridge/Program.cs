using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using VRE.Vridge.API.Client.Proxy;

namespace PsMoveServiceRiftCatBridge
{
    class Program
    {
        static void Main(string[] args)
        {
            var apiClient = new APIClient();
            var hmdService = new HMDService(apiClient);

            Console.WriteLine("Starting RiftCat connector");
            PsMoveServiceClientWrapper client = null;
            try
            {
                bool isRunning = true;
                client = new PsMoveServiceClientWrapper();
                if (client.Startup())
                {
                    while (isRunning)
                    {
                        var result = client.Update();
                        hmdService.SetPositionData(result.X/100, result.Y/100, result.Z/100);
                        hmdService.Recenter(result.Recenter);
                        Console.WriteLine($"X: {result.X}, Y: {result.Y}, Z: {result.Z}, Recenter: {result.Recenter}");
                    }
                }
                else
                {
                    Console.WriteLine("Failed to start Move Client");
                    Console.ReadKey();
                }
            }
             catch(Exception e)
            {
                Console.WriteLine($"Error occured: { e.Message}");

            }

            try
            {
                client?.Shutdown();
            }
            catch(Exception e)
            {
                Console.WriteLine($"Error on shutdown: {e.Message}");

            }
            Console.ReadKey();
        }
    }
}
