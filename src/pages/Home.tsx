import Widget from "../Components/Widget";
import TitleBar from "../Components/TitleBar";

const Home = () => {
  return (
    <>
      <TitleBar title="Intelligarden Hub" />
      <div className="container">
        <div className="row">
          <Widget
            title="WiFi Settings"
            info="Change WiFi or Hotspot settings"
            link="/wifi-settings"
            bgColor="#ffebee"
          />
          <Widget
            title="Manage Schedulers"
            info="Schedule your irrigation, lighting and etc..."
            link="/schedulers"
            bgColor="#e3f2fd"
          />
          <Widget
            title="FusionBus Devices"
            info="Setup External Sensors, Actuators and etc..."
            link="/fusionbus-devices"
            bgColor="#e8f5e9"
          />
          <Widget
            title="Manage Thermostats"
            info="Setup your Temperature Controllers"
            link="/thermostats"
            bgColor="#ffaaaa"
          />
          <Widget
            title="Firebase Settings"
            info="Configure Firebase"
            link="/firebase-settings"
            bgColor="#fff3e0"
          />
          <Widget
            title="Water Sensor Settings"
            info="Pond Inlet Water Data Logging"
            link="/"
            bgColor="#a3dafd"
          />
          <Widget
            title="Display Settings"
            info="Config Display Properties"
            link="/display-settings"
            bgColor="#f3e5f5"
          />
          <Widget
            title="Data Logging"
            info="Config Common Data Logging Settings"
            link="/data-logging"
            bgColor="#aaebee"
          />
          <Widget
            title="Setup Slave Nodes"
            info="Connect to other esp32 nodes"
            link="/"
            bgColor="#ffee99"
          />
          <Widget
            title="Setup Valves"
            info="Connect to a Wired / Wireless Valve"
            link="/"
            bgColor="#ffcc99"
          />
          <Widget
            title="System Region"
            info="Configure System Timezone"
            link="/timezone-settings"
            bgColor="#cccc99"
          />
          <Widget
            title="Signal Manager"
            info="Link Subsystems | Unified Signaling System"
            link="/signal-manager"
            bgColor="#cccc66"
          />
          <Widget
            title="Manage I/Os"
            info="Setup Gpios & Ports"
            link="/io-manager"
            bgColor="#bbdd88"
          />
        </div>
      </div>
    </>
  );
};

export default Home;
