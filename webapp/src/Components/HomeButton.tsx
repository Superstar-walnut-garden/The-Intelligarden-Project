import React from "react";
import { Link } from "react-router-dom";
import { House } from "react-bootstrap-icons";

const HomeButton: React.FC = () => {
  return (
    <Link
      to="/"
      className="btn btn-primary align-items-center align-text-center justify-contents-center  rounded-circle m-1"
    >
      <House />
    </Link>
  );
};

export default HomeButton;
