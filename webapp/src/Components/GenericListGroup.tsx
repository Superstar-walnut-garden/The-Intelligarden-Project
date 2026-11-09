import React from "react";
import "bootstrap/dist/css/bootstrap.min.css";

interface GenericListGroupProps<T> {
    items: T[];
    onCreate: (item: T) => void;
    onSave: (item: T) => void;
    onRemove: (id: number) => void;
    renderItem: (item: T, onSave: (item: T) => void, onRemove: (id: number) => void) => React.ReactNode;
    defaultItemProps: Partial<T>; // Add this prop to specify default properties for a new item
}

const GenericListGroup = <T extends { id: number }>({
    items,
    onCreate,
    onSave,
    onRemove,
    renderItem,
    defaultItemProps, // Destructure the new prop
}: GenericListGroupProps<T>) => {
    const handleAddItem = () => {
        const newItem: T = {
            ...defaultItemProps,
            id: nextId(),
        } as T;
        onCreate(newItem);
    };

    function nextId() {
        let newId = 1;
        while (items.some(item => item.id === newId)) {
            newId++;
        }
        return newId;
    }

    return (
        <div
            className="d-flex flex-column align-items-center mb-3 border p-2 rounded p-2 bg-light"
            style={{ width: "fit-content", height: "fit-content" }}
        >
            <div
                className="list-group overflow-auto"
                style={{ maxHeight: "400px", width: "fit-content" }}
            >
                {items.map((item) => (
                    <div key={item.id} className="list-group-item">
                        {renderItem(item, onSave, onRemove)}
                    </div>
                ))}
            </div>
            <div className="d-flex align-items-center w-100">
                <button className="btn btn-primary m-4 w-100" onClick={handleAddItem}>
                    Add New Item
                </button>
            </div>
        </div>
    );
};

export default GenericListGroup;