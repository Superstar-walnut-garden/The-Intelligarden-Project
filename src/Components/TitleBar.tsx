import "bootstrap/dist/css/bootstrap.min.css";
import "./WeekSelector.css";
import HomeButton from "./HomeButton";

interface TitleBarProps {
  title?: string;
}

const TitleBar: React.FC<TitleBarProps> = ({ title = "Unnamed Title" }) => {
  return (
    <>
      <div className="d-flex align-items-center border-bottom mb-4 bg-gradient bg-light">
        <div className="position-absolute start-0">
          <HomeButton />
        </div>
        <div className="mx-auto">
          <h1 className=" text-center">{title}</h1>
        </div>
      </div>
    </>
  );
};

export default TitleBar;
