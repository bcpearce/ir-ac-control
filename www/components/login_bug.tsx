import styles from '@/styles/Home.module.css'

export default function LoginBug(
    {handleConnect, handleDisconnect, 
    connectionStatus, connectionError, connectionBroker}) {
    
    function handleSubmitLogin(e) {
        e.preventDefault();

        const form = e.target;
        const formData = new FormData(form);
        const formJson = Object.fromEntries(formData.entries());

        console.log(formJson.broker)
        console.log(formJson.userName)
        console.log(formJson.userKey)

        const options = {username: formJson.userName, password: formJson.userKey};

        handleConnect(formJson.broker, options);
    }

    function handleSubmitLogout(e) {
        console.log("Disconnecting");
        handleDisconnect();
    }

    function renderLogin() {
        return (
            <form onSubmit={handleSubmitLogin}>
            <h2>Login: </h2>
            <p>
                <label><b>Broker: </b><input name="broker" defaultValue="mqtt://"/></label><br />
                <label>User Name: <input name="userName" /></label><br />
                <label>User Key: <input name="userKey" type="password"/></label><br />
                <button type="submit">&gt;&gt;&gt;</button>
            </p>
            </form>
        );
    }

    function renderLogout() {
        return (
            <>
            <h2>Connected to Broker:</h2>
            <p><span><b>{connectionBroker}</b> </span><br />
            <button onClick={handleSubmitLogout}>Logout</button>
            </p>
            </>   
        )
    }

    return (
        <div className={styles.card}>
            {connectionStatus ? renderLogout() : renderLogin()}
            {Object.keys(connectionError).length !== 0 && <p className={styles.error}>{connectionError.toString()}</p>}
        </div>
    );
}