import { FC, useEffect } from "react";
import { useLiveShare } from "./FileContentContext";

const HostLiveShare: FC = () => {
    const { fileData, peerId, initialize, setFileData } = useLiveShare();
    // Initialize the peer connection when the component mounts
    useEffect(() => {
        initialize();
        setFileData("new value");
    }, []);

    return (
        <div className="p-4 border rounded">
            <h2 className="text-xl mb-4">Live Share Host</h2>
            {peerId ? (
                <div>
                    <p className="mb-2">Your session ID: <strong>{peerId}</strong></p>
                    <p className="text-sm text-gray-600">Share this ID with others to let them join your session</p>
                    
                    <div className="mt-4">
                        <h3 className="font-medium">Current File Content:</h3>
                        <div className="mt-2 p-2 bg-gray-100 rounded">
                            <pre className="whitespace-pre-wrap">{fileData || "[Empty file]"}</pre>
                        </div>
                    </div>
                </div>
            ) : (
                <p>Initializing connection...</p>
            )}
        </div>
    );
};

export default HostLiveShare;