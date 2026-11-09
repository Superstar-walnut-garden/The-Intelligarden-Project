import React from "react";
import { BrowserRouter as Router, Route, Routes } from "react-router-dom";
import PumpSettings from "./pages/PumpSettings";
import Home from "./pages/Home";
import WifiSettings from "./pages/WifiSettings";
import FirebaseSettings from "./pages/FirebaseSettings";
import SignalManager from "./pages/SignalManager";
import PinManagerPage from "./pages/PinManagerPage";
import DisplaySettings from "./pages/DisplaySettings";
import ThermostatManagerPage from "./pages/ThermostatManagerPage";
import TimezoneSettings from "./pages/TimezoneSettings";
import DataLoggingPage from "./pages/DataLoggingPage";
import FusionBusSettings from "./pages/FusionBusSettings";

const App: React.FC = () => {
  return (
    <Router>
      <Routes>
        <Route path="/" element={<Home />} />
        <Route path="/schedulers" element={<PumpSettings />} />
        <Route path="/wifi-settings" element={<WifiSettings />} />
        <Route path="/firebase-settings" element={<FirebaseSettings />} />
        <Route path="/fusionbus-devices" element={<FusionBusSettings />} />
        <Route path="/signal-manager" element={<SignalManager />} />
        <Route path="/io-manager" element={<PinManagerPage />} />
        <Route path="/display-settings" element={<DisplaySettings />} />
        <Route path="thermostats" element={<ThermostatManagerPage />} />
        <Route path="timezone-settings" element={<TimezoneSettings />} />
        <Route path="data-logging" element={<DataLoggingPage />} />
        <Route path="*" element={<Home />} /> {/* Fallback route */}
      </Routes>
    </Router>
  );
};

export default App;
