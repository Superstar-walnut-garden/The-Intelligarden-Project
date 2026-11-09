// src/pages/WifiConfig.tsx
import React, { useState } from "react";
import { Tabs, Tab } from "react-bootstrap";
import WifiConfigForm from "../Components/WifiConfigForm";
import HotspotConfigForm from "../Components/HotspotConfigForm";
import TitleBar from "../Components/TitleBar";

const WifiSettings: React.FC = () => {
  const [key, setKey] = useState<string>("connect");

  return (
    <>
      <TitleBar title="Wifi Settings" />
      <div className="container mt-4">
        <Tabs activeKey={key} onSelect={(k) => setKey(k || "connect")}>
          <Tab eventKey="connect" title="Wifi Setup">
            <WifiConfigForm />
          </Tab>
          <Tab eventKey="hotspot" title="Hotspot Setup">
            <HotspotConfigForm />
          </Tab>
        </Tabs>
      </div>
    </>
  );
};

export default WifiSettings;
