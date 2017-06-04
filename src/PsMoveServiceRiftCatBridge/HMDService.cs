using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using VRE.Vridge.API.Client.Proxy;
using VRE.Vridge.API.Client.Proxy.HeadTracking;

namespace PsMoveServiceRiftCatBridge
{
    public class HMDService
    {
        private HeadTrackingProxy _headTrackingProxy; 
        private APIClient _apiClient;
        private object _broadCastProxy;

        public HMDService(APIClient apiClient)
        {
            _apiClient = apiClient;
            ConnectOrReconnect();
        }

        public void SetPositionData(float x, float y, float z)
        {
            _headTrackingProxy.SetPosition(x, y, z);
        }

        private void ConnectOrReconnect()
        {
            try
            {
                // Close active connections (if restarting)
                _apiClient.DisconnectHeadTrackingProxy();
                _apiClient.DisconnectControllerProxy();
                _apiClient.DisconnectBroadcastProxy();

                // Give it some time to clean up
                Thread.Sleep(10);

                // Connect to the services
                _headTrackingProxy = _apiClient.ConnectHeadTrackingProxy();

                _broadCastProxy = _apiClient.ConnectToBroadcaster();

                _headTrackingProxy.ChangeTrackingState(true);
            }
            catch (Exception x)
            {
                Console.WriteLine($"Error: {x.Message}");
            }
        }

        public void Recenter(bool recenter)
        {
            if(recenter)
                _headTrackingProxy.RecenterView();
        }
    }
}
