import React, { useState, useEffect } from "react";
import GPIOListGroup from "../Components/GPIOListGroup";
import TitleBar from "../Components/TitleBar";
import { GPIOItemProps } from "../Components/GPIOItem";
import * as ApiService from "../api/apiService";

const PinManagerPage: React.FC = () => {
  const [GPIOItems, setGPIOItems] = useState<GPIOItemProps[]>([]);
  useEffect(() => {
    fetchList();
  }, []);

  const fetchList = async () => {
    try {
      const data = await ApiService.getGPIOList();
      setGPIOItems(data);
    } catch (error) {
      console.error("Error fetching schedule list:", error);
    }
  };

  const createGPIO = async (newIO: GPIOItemProps) => {
    const newIOData = { ...newIO };
    try {
      await ApiService.createGPIO(newIOData);
    } catch (error) {
      console.error("Error creating GPIO:", error);
    }
    fetchList(); //get updated list
  };

  const deleteGPIO = async (id: number) => {
    try {
      await ApiService.deleteGPIO(id);
    } catch (error) {
      console.error("Error deleting GPIO:", error);
    }
    fetchList(); //get updated list
  };

  const modifyGPIO = async (updatedItem: GPIOItemProps) => {
    try {
      await ApiService.modifyGPIO(updatedItem.id, updatedItem);
    } catch (error) {
      console.error("Error modifying GPIO:", error);
    }
    fetchList(); // get updated list
  };

  return (
    <>
      <div className="flex-d align-items-center">
        <TitleBar title="Manage I/Os" />
        <div className="d-flex justify-content-center align-items-center">
          <GPIOListGroup
            items={GPIOItems}
            onSave={modifyGPIO}
            onRemove={deleteGPIO}
            onCreate={createGPIO}
          />
        </div>
      </div>
    </>
  );
};

export default PinManagerPage;
