import styles from '@/styles/Home.module.css'

export default function AcBug({zone}) {
    return (
        <div className={styles.card}>
        <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css"/>
            <h2>Air Conditioner</h2>
            <h3>Zone: {zone}</h3>
            <form>
            <p>
                <label><b>On: </b>
                    <input type="checkbox" name="on" />
                </label>
            </p>
            <p>
                <b>Temperature: </b>
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
                    <i class="fa fa-moon-o"></i>&nbsp;&nbsp;
                </label>
                <label>
                    <input type="radio" name="fan" value="Auto" />
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

export function AcBugContainer({acRemoteState}) {

    const listBugs = Object.keys(acRemoteState).map((keyName, i) => {
            return (<AcBug
              zone={acRemoteState[keyName].zone}
              key={i}/>)
          });

    return (
        <div className={styles.description}>{listBugs}</div>
    );
}