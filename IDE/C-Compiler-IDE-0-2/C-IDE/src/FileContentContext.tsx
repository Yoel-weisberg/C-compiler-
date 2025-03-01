import { createContext, useContext, useState, ReactNode, useEffect, useRef } from "react";
import { Peer, DataConnection } from "peerjs";

// Define the shape of the context state
interface LiveShareContextType {
    fileData: string;
    setFileData: (content: string) => void;
    connections: DataConnection[];
    peer: Peer | null;
    peerId: string | null;
    initialize: () => void;
    isHost: boolean;
    setIsHost: (content: boolean) => void;
}

interface DataType {
    type: string;
    data:string
}

// Create the context with a default undefined value
const LiveShareContext = createContext<LiveShareContextType | undefined>(undefined);

// Create a provider component
export const LiveShareProvider = ({ children }: { children: ReactNode }) => {
    const [fileData, setFileData] = useState<string>("");
    const [peer, setPeer] = useState<Peer | null>(null);
    const [peerId, setPeerId] = useState<string | null>(null);
    const [connections, setConnections] = useState<DataConnection[]>([]);
    const [isHost, setIsHost] = useState<boolean>(false)
    const previousFileData = useRef<string>("");
    
    // Initialize PeerJS
    const initialize = () => {
        const newPeer = new Peer();
        
        newPeer.on('open', (id) => {
            console.log('My peer ID is:', id);
            setPeerId(id);
        });
        
        newPeer.on('connection', (conn) => {
            conn.on('open', () => {
                // Send current file data to new connection
                conn.send({ type: 'fileData', data: fileData });
                
                // Add connection to our connections list
                setConnections(prev => [...prev, conn]);
                
                // Handle data from this connection
                conn.on('data', (data : any) => {
                    // Process incoming data if needed
                    if (data.type === 'fileData') {
                        setFileData(data.data);
                    }
                });
                
                // Handle connection closed
                conn.on('close', () => {
                    setConnections(prev => prev.filter(c => c !== conn));
                });
            });
        });
        
        setPeer(newPeer);
    };
    
    // Track changes to fileData and broadcast to all connections
    useEffect(() => {
        // Skip first render 
        if (previousFileData.current !== fileData) {
            // Broadcast file data changes to all connections
            connections.forEach(conn => {
                if (conn.open) {
                    conn.send({ type: 'fileData', data: fileData });
                }
            });
            
            // Update previous value
            previousFileData.current = fileData;
        }
    }, [fileData, connections]);
    
    // Custom setFileData that handles updates properly
    const updateFileData = (content: string) => {
        setFileData(content);
    };
    
    return (
        <LiveShareContext.Provider value={{ 
            fileData, 
            setFileData: updateFileData, 
            connections,
            peer,
            peerId,
            initialize,
            isHost,
            setIsHost
        }}>
            {children}
        </LiveShareContext.Provider>
    );
};

// Custom hook for using the context
export const useLiveShare = (): LiveShareContextType => {
    const context = useContext(LiveShareContext);
    if (!context) {
        throw new Error("useLiveShare must be used within a LiveShareProvider");
    }
    return context;
};