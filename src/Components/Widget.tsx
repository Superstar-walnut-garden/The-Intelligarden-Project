// src/components/Widget.tsx
import React from 'react';
import { Link } from 'react-router-dom';

interface WidgetProps {
  title: string;
  info: string;
  link: string;
  bgColor?: string; // Optional prop for background color
}

const Widget: React.FC<WidgetProps> = ({ title, info, link, bgColor = '#f8f9fa' }) => {
  return (
    <Link to={link} className="col-md-4 mb-4 text-decoration-none">
      <div className="card widget-card" style={{ backgroundColor: bgColor }}>
        <div className="card-body d-flex flex-column justify-content-center align-items-center">
          <h5 className="card-title">{title}</h5>
          <p className="card-text">{info}</p>
        </div>
      </div>
    </Link>
  );
};

export default Widget;
