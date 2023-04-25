import styles from '@/styles/Home.module.css'
import switchStyle from '@/styles/switch.css'

export default function AcBug({zoneKey, zoneName, client}) {

    const topicBase = 'zone/' + zoneKey + '/';
    const options = {qos:2};

    function PublishAcInfo(topic, value) {
        if (client) {
            client.publish(topic, value, options);
            console.log("Published", value, "to", topic);
        }
        else {
            console.log("No MQTT client available to send data.");
        }
    }

    function OnSendAcTx(e) {
        e.preventDefault();
        console.log("Sending AC Data", e);
        PublishAcInfo(topicBase + 'ac/data/onOff', e.target.on.checked ? "on" : "off");
        const temperatureDegC = (parseFloat(e.target.temperature.value) - 32) * 0.5555;
        PublishAcInfo(topicBase + 'ac/data/temperatureDegC', temperatureDegC.toString());
        PublishAcInfo(topicBase + 'ac/data/fan', e.target.fan.value);
        PublishAcInfo(topicBase + 'ac/data/mode', e.target.mode.value);
        PublishAcInfo(topicBase + 'ac/tx', "tx", options);
    }

    return (
        <div className={styles.card}>
        <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css"/>
            <h2>Air Conditioner</h2>
            <h3>Zone: {zoneName}</h3>
            <form onSubmit={OnSendAcTx}>
            <p>
                <label><b>On/Off: </b>
                    <input type="checkbox" name="on" value="0" id="on_off_check" />
                </label>
                <span className={styles.value}><input type="submit" value="  Send TX  "/></span>
            </p>
            <p>
                <b>Temperature &#8457;: </b>
                <label><input name="temperature" className={styles.narrowInput}/></label>
            </p>
            <p><b>Fan: </b><br />
                <label>
                    <input type="radio" name="fan" value="1" />
                    1&nbsp;&nbsp;
                </label>
                <label>
                    <input type="radio" name="fan" value="2" />
                    2&nbsp;&nbsp;  
                </label>
                <label>
                    <input type="radio" name="fan" value="3" />
                    3&nbsp;&nbsp;
                </label>
                <label>
                    <input type="radio" name="fan" value="4" />
                    4&nbsp;&nbsp;
                </label>
                <label>
                    <input type="radio" name="fan" value="5" />
                    5&nbsp;&nbsp;
                </label>
                <label>
                    <input type="radio" name="fan" value="Silent" />
                    <i className={"fa fa-moon-o"}></i>&nbsp;&nbsp;
                </label>
                <label>
                    <input type="radio" name="fan" value="Auto"/>
                    A&nbsp;&nbsp;
                </label>
            </p>
            <p><b>Mode: </b><br />
                <label>
                    <input type="radio" name="mode" value="dry" />
                    Dry&nbsp;&nbsp;
                </label>
                <label>
                    <input type="radio" name="mode" value="heat" />
                    Heat&nbsp;&nbsp;
                </label>
                <label>
                    <input type="radio" name="mode" value="cool" />
                    Cool&nbsp;&nbsp;
                </label>
                <br/>
                <label>
                    <input type="radio" name="mode" value="fan" />
                    Fan&nbsp;&nbsp;
                </label>
                <label>
                    <input type="radio" name="mode" value="auto" />
                    Auto&nbsp;&nbsp;
                </label>
            </p>

            </form>
        </div>
    );
}

export function AcBugContainer({acAdvert, zoneNames, client}) {

    const listBugs = Object.keys(acAdvert).map((keyName, i) => {
            return (
                <div key={i}>
                {acAdvert[keyName] == 1 && 
                <AcBug 
                    zoneKey={keyName}
                    zoneName={zoneNames[keyName]}  
                    client={client}/>}
                </div>)
          });

    return (
        <div className={styles.description}>{listBugs}</div>
    );
}