import React from "react";

interface BadgeIndicatorProps {
  label: string;
  status: boolean;
  successText?: string;
  failureText?: string;
}

const BadgeIndicator: React.FC<BadgeIndicatorProps> = ({
  label,
  status,
  successText = "OK",
  failureText = "Not Found",
}) => {
  return (
    <div className="d-flex align-items-center">
      <span className="me-2">{label}:</span>
      <span className={`badge ${status ? "bg-success" : "bg-danger"}`}>
        {status ? successText : failureText}
      </span>
    </div>
  );
};

export default BadgeIndicator;
