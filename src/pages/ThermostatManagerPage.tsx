import React, { useState, useEffect } from "react";
import GenericListGroup from "../Components/GenericListGroup";
import TitleBar from "../Components/TitleBar";
import ThermostatItem from "../Components/ThermostatItem";
import * as ApiService from "../api/apiService";
import { ThermostatApiData } from "../api/apiService";
import { TempSensorApiData } from "../api/apiService";

const ThermostatManagerPage: React.FC = () => {
  const [items, setItems] = useState<ThermostatApiData[]>([]);
  const [sensorList, setSensorList] = useState<TempSensorApiData[]>([]);
  useEffect(() => {
    fetchList();
  }, []);

  useEffect(() => {
    const getSensorList = async () => {
      setSensorList(await ApiService.getFusionBusList());
    };
    getSensorList();
  }, []);

  const fetchList = async () => {
    try {
      const data = await ApiService.getThermostatList();
      setItems(data);
    } catch (error) {
      console.error("Error fetching schedule list:", error);
    }
  };

  const createThermostate = async (newIO: ThermostatApiData) => {
    const newIOData = { ...newIO };
    try {
      await ApiService.createThermostate(newIOData);
    } catch (error) {
      console.error("Error creating Thermostat:", error);
    }
    fetchList(); //get updated list
  };

  const deleteThermostat = async (id: number) => {
    try {
      await ApiService.deleteThermostat(id);
    } catch (error) {
      console.error("Error deleting Thermostat:", error);
    }
    fetchList(); //get updated list
  };

  const modifyThermostat = async (updatedItem: ThermostatApiData) => {
    try {
      await ApiService.modifyThermostat(updatedItem.id, updatedItem);
    } catch (error) {
      console.error("Error modifying Thermostat:", error);
    }
    fetchList(); // get updated list
  };

  return (
    <>
      <div className="flex-d align-items-center">
        <TitleBar title="Manage Thermostats" />
        <div className="d-flex justify-content-center align-items-center">
          <GenericListGroup
            items={items}
            onCreate={createThermostate}
            onRemove={deleteThermostat}
            onSave={modifyThermostat}
            renderItem={(item, onSave, onRemove) => (
              <ThermostatItem
                item={item}
                sensorList={sensorList}
                onSave={onSave}
                onRemove={onRemove}
              />
            )}
            defaultItemProps={{
              id: 0,
              name: "Untitled Thermostat",
              setpoint: 20,
              event_id: 0,
              status: false,
              sensor: "-1",
            }}
          />
        </div>
      </div>
    </>
  );
};

export default ThermostatManagerPage;
