// src/components/WifiConfigForm.tsx
import React, { useState, useEffect } from "react";
import { Form, Button, Row, Col } from "react-bootstrap";
import TitleBar from "../Components/TitleBar";
import * as ApiService from "../api/apiService"
import { FirebaseSettingsData } from "../api/apiService";

const FirebaseSettings: React.FC = () => {
  const [firebaseSettingsData, setFirebaseSettingsData] =
    useState<FirebaseSettingsData>({
      apiKey: "",
      databaseURL: "",
      userEmail: "",
      userPassword: "",
      databaseRootName: "",
      enabled: false,
    });


  useEffect(() => {
    const getData = async () => {
      setFirebaseSettingsData(await ApiService.getFirebaseData());
      setFirebaseEnabled(firebaseSettingsData.enabled);
    };
    getData();
  }, []);

  const [firebaseEnabled, setFirebaseEnabled] = useState<boolean>(firebaseSettingsData.enabled);

  const handleSave = () => {
    firebaseSettingsData.enabled = firebaseEnabled;
    ApiService.setFirebaseData(firebaseSettingsData);
  };
  const handleEnableToggle = () => {
    setFirebaseEnabled(!firebaseEnabled);
  };

  return (
    <>
      {" "}
      <TitleBar title="Firebase Settings" />
      <Form className="mt-3">
        <>
          <Form.Group as={Row} controlId="formFirebase">
            <Form.Label column sm={2}>
              Data Logging is {firebaseEnabled ? "Enabled" : "Disabled"}
            </Form.Label>
            <Col sm={10}>
              <Form.Check
                type="switch"
                checked={firebaseEnabled}
                onChange={handleEnableToggle}
              />
            </Col>
          </Form.Group>
          {firebaseEnabled && (
            <>
              <Form.Group as={Row} controlId="formApiKey">
                <Form.Label column sm={2}>
                  apiKey
                </Form.Label>
                <Col sm={10}>
                  <Form.Control
                    type="text"
                    value={firebaseSettingsData.apiKey || ""}
                    onChange={(e) =>
                      setFirebaseSettingsData({
                        ...firebaseSettingsData,
                        apiKey: e.target.value,
                      })
                    }
                  />
                </Col>
              </Form.Group>
              <Form.Group as={Row} controlId="formDatabaseURL">
                <Form.Label column sm={2}>
                  databaseURL
                </Form.Label>
                <Col sm={10}>
                  <Form.Control
                    type="text"
                    value={firebaseSettingsData.databaseURL || ""}
                    onChange={(e) =>
                      setFirebaseSettingsData({
                        ...firebaseSettingsData,
                        databaseURL: e.target.value,
                      })
                    }
                  />
                </Col>
              </Form.Group>
              <Form.Group as={Row} controlId="formUserEmail">
                <Form.Label column sm={2}>
                  userEmail
                </Form.Label>
                <Col sm={10}>
                  <Form.Control
                    type="text"
                    value={firebaseSettingsData.userEmail || ""}
                    onChange={(e) =>
                      setFirebaseSettingsData({
                        ...firebaseSettingsData,
                        userEmail: e.target.value,
                      })
                    }
                  />
                </Col>
              </Form.Group>
              <Form.Group as={Row} controlId="formUserPassword">
                <Form.Label column sm={2}>
                  userPassword
                </Form.Label>
                <Col sm={10}>
                  <Form.Control
                    type="text"
                    value={firebaseSettingsData.userPassword || ""}
                    onChange={(e) =>
                      setFirebaseSettingsData({
                        ...firebaseSettingsData,
                        userPassword: e.target.value,
                      })
                    }
                  />
                </Col>
              </Form.Group>
              <Form.Group as={Row} controlId="formDatabaseRootName">
                <Form.Label column sm={2}>
                  databaseRootName
                </Form.Label>
                <Col sm={10}>
                  <Form.Control
                    type="text"
                    value={firebaseSettingsData.databaseRootName || ""}
                    onChange={(e) =>
                      setFirebaseSettingsData({
                        ...firebaseSettingsData,
                        databaseRootName: e.target.value,
                      })
                    }
                  />
                </Col>
              </Form.Group>
            </>
          )}
        </>

        <Button variant="primary" onClick={handleSave} className="mt-3">
          Apply / Save
        </Button>
      </Form>
    </>
  );
};

export default FirebaseSettings;
