// src/components/HotspotConfigForm.tsx
import React, { useState, useEffect } from 'react';
import { Form, Button, Row, Col } from 'react-bootstrap';
import * as ApiService from "../api/apiService";
import { HotspotConfig } from "../api/apiService";

const HotspotConfigForm: React.FC = () => {
  const [hotspotConfig, setHotspotConfig] = useState<HotspotConfig>({
    ssid: '',
    password: '',
  });

  useEffect(() => {
    const getData = async () => {
      setHotspotConfig(await ApiService.getHotspotConfig());
    }
    getData();
  }, []);

  const handleSaveHotspotConfig = () => {
    ApiService.setHotspotConfig(hotspotConfig);
    console.log('Hotspot config saved');
  };

  return (
    <Form className="mt-3">
      <Form.Group as={Row} controlId="formHotspotSsid">
        <Form.Label column sm={2}>SSID</Form.Label>
        <Col sm={10}>
          <Form.Control
            type="text"
            value={hotspotConfig.ssid || ''}
            onChange={(e) => setHotspotConfig({ ...hotspotConfig, ssid: e.target.value })}
          />
        </Col>
      </Form.Group>
      <Form.Group as={Row} controlId="formHotspotPassword">
        <Form.Label column sm={2}>Password</Form.Label>
        <Col sm={10}>
          <Form.Control
            type="password"
            value={hotspotConfig.password || ''}
            onChange={(e) => setHotspotConfig({ ...hotspotConfig, password: e.target.value })}
          />
        </Col>
      </Form.Group>
      <Button variant="primary" onClick={handleSaveHotspotConfig} className="mt-3">
        Save
      </Button>
    </Form>
  );
};

export default HotspotConfigForm;
