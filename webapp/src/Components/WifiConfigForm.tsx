// src/components/WifiConfigForm.tsx
import React, { useState, useEffect } from 'react';
import { Form, Button, Row, Col } from 'react-bootstrap';
import * as ApiService from "../api/apiService";
import { WifiState } from "../api/apiService";

const WifiConfigForm: React.FC = () => {
  const [wifiState, setWifiState] = useState<WifiState>({
    ssid: '',
    password: '',
    dhcpEnabled: false,
    on: false,
  });

  useEffect(() => {
    const getData = async () => {
      setWifiState(await ApiService.getWifiState());
      console.log(wifiState.on ? "wifi is on": "wifi is off");
    };
    getData();
  }, []);


  const handleDhcpToggle = () => {
    setWifiState({ ...wifiState, dhcpEnabled: !wifiState.dhcpEnabled});
  };

  const handleWifiOnToggle = () => {
    setWifiState(prevState => {
      const newState = { ...prevState, on: !prevState.on };
      console.log(newState.on ? "wifi is on" : "wifi is off");
      ApiService.setWifiConfig(newState);
      return newState;
    });
  };

  const handleSaveWifiConfig = () => {
    ApiService.setWifiConfig(wifiState);
    ApiService.rebootSystem();
    console.log('WiFi config saved. reboot requested!');
  };

  return (
    <Form className="mt-3">
      <Form.Group as={Row} controlId="formDhcp">
        <Form.Label column sm={2}>WiFi</Form.Label>
        <Col sm={10}>
          <Form.Check
            type="switch"
            checked={wifiState.on}
            onChange={handleWifiOnToggle}
          />
        </Col>
      </Form.Group>
      {wifiState.on && (
        <>
          <Form.Group as={Row} controlId="formSsid">
            <Form.Label column sm={2}>SSID</Form.Label>
            <Col sm={10}>
              <Form.Control
                type="text"
                value={wifiState.ssid || ''}
                onChange={(e) => setWifiState({ ...wifiState, ssid: e.target.value })}
              />
            </Col>
          </Form.Group>
          <Form.Group as={Row} controlId="formPassword">
            <Form.Label column sm={2}>Password</Form.Label>
            <Col sm={10}>
              <Form.Control
                type="password"
                value={wifiState.password || ''}
                onChange={(e) => setWifiState({ ...wifiState, password: e.target.value })}
              />
            </Col>
          </Form.Group>
          <Form.Group as={Row} controlId="formDhcp">
            <Form.Label column sm={2}>DHCP</Form.Label>
            <Col sm={10}>
              <Form.Check
                type="switch"
                checked={wifiState.dhcpEnabled}
                onChange={handleDhcpToggle}
              />
            </Col>
          </Form.Group>
          {!wifiState.dhcpEnabled && (
            <>
              <Form.Group as={Row} controlId="formIp">
                <Form.Label column sm={2}>IP Address</Form.Label>
                <Col sm={10}>
                  <Form.Control
                    type="text"
                    value={wifiState.ip || ''}
                    onChange={(e) => setWifiState({ ...wifiState, ip: e.target.value })}
                  />
                </Col>
              </Form.Group>
              <Form.Group as={Row} controlId="formSubnet">
                <Form.Label column sm={2}>Subnet Mask</Form.Label>
                <Col sm={10}>
                  <Form.Control
                    type="text"
                    value={wifiState.subnet || ''}
                    onChange={(e) => setWifiState({ ...wifiState, subnet: e.target.value })}
                  />
                </Col>
              </Form.Group>
              <Form.Group as={Row} controlId="formGateway">
                <Form.Label column sm={2}>Gateway</Form.Label>
                <Col sm={10}>
                  <Form.Control
                    type="text"
                    value={wifiState.gateway || ''}
                    onChange={(e) => setWifiState({ ...wifiState, gateway: e.target.value })}
                  />
                </Col>
              </Form.Group>
              <Form.Group as={Row} controlId="formPrimaryDNS">
                <Form.Label column sm={2}>Primary DNS</Form.Label>
                <Col sm={10}>
                  <Form.Control
                    type="text"
                    value={wifiState.primaryDNS || ''}
                    onChange={(e) => setWifiState({ ...wifiState, primaryDNS: e.target.value })}
                  />
                </Col>
              </Form.Group>
              <Form.Group as={Row} controlId="formSecondaryDNS">
                <Form.Label column sm={2}>Secondary DNS</Form.Label>
                <Col sm={10}>
                  <Form.Control
                    type="text"
                    value={wifiState.secondaryDNS || ''}
                    onChange={(e) => setWifiState({ ...wifiState, secondaryDNS: e.target.value })}
                  />
                </Col>
              </Form.Group>
            </>
          )}
        </>)} 

      <Button variant="primary" onClick={handleSaveWifiConfig} className="mt-3">
        Save & Restart
      </Button>
    </Form>
  );
};

export default WifiConfigForm;
