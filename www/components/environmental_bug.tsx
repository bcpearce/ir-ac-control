import styles from '@/styles/Home.module.css'
import moment from 'moment';

export default function EnvironmentalBug({zone, temperature_degC, humidity_pct, battery_cell_percent, battery_voltage, last_updated}) {
    return (
        <div className={styles.card}>
            <h2>Environment</h2>
            <h3>Zone: {zone}</h3>
            { temperature_degC && 
                <p><b>Temperature &#8457;:</b><span className={styles.value}>{(temperature_degC * 9 / 5 + 32).toFixed(1)}</span></p>
            }
            { humidity_pct &&
                <p><b>Humidity &#x25;:</b> <span className={styles.value}>{humidity_pct.toFixed(1)}</span></p>
            }
            { (battery_cell_percent && battery_voltage) &&
                <>
                <hr />
                <p><b>Battery &#x25;:</b> <span className={styles.value}>{battery_cell_percent.toFixed(0)}</span></p>
                <p><b>Voltage (V):</b> <span className={styles.value}>{battery_voltage.toFixed(2)}</span></p>
                </>
            }
            { last_updated &&
                <>
                <hr />
                <p><b>Last Updated:</b> <span className={styles.value}>{moment(last_updated).fromNow()}</span></p>
                </>
            }
        </div>
    );
}

export function EnvironmentalBugContainer(
    {
        zoneNames,
        humidityPct,
        temperatureDegC,
        batteryCellPct,
        batteryVoltage
    }) {

    const listBugs = Object.keys(zoneNames).map((keyName, i) => {
            return (<EnvironmentalBug 
              zone={zoneNames[keyName]}
              temperature_degC={temperatureDegC[keyName]}
              humidity_pct={humidityPct[keyName]}
              battery_cell_percent={batteryCellPct[keyName]}
              battery_voltage={batteryVoltage[keyName]}
              key={i}/>)
          });

    return (
        <div className={styles.grid, styles.description}>{listBugs}</div>
    );
}