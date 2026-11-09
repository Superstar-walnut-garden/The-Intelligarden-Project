import "bootstrap/dist/css/bootstrap.min.css";
import { useState } from "react";

interface ItemTitleProps {
  title?: string;
  isEditing?: boolean;
  onTextChange?: (text: string) => void;
  placeholder?: string;
}

const ItemTitle: React.FC<ItemTitleProps> = ({
  title = "Unnamed Title",
  isEditing = false,
  onTextChange,
  placeholder = "",
}) => {
  const [titleText, setTitleText] = useState(title);
  return (
    <>
      <div className="d-flex align-items-center w-100 mb-1 mx-auto text-center justify-content-center bg-secondary rounded-top">
        <input
          title="name"
          type="text"
          placeholder={placeholder}
          className={`w-auto mx-1 bg-secondary border-0 text-white text-center fw-bold ${
            isEditing ? "form-control bg-primary" : "form-control-plaintext"
          }`}
          value={titleText}
          onChange={(e: React.ChangeEvent<HTMLInputElement>) => {
            setTitleText(e.target.value);
            if (onTextChange) onTextChange(String(e.target.value));
          }}
          disabled={!isEditing}
        />
        <style>{`input::placeholder {color: darkgray}`}</style>
      </div>
    </>
  );
};

export default ItemTitle;
