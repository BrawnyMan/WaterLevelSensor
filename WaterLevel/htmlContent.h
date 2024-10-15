// htmlContent.h
#ifndef HTML_CONTENT_H
#define HTML_CONTENT_H

const char* htmlContent = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Water Level Meter</title>
    <script src='https://cdn.jsdelivr.net/npm/chart.js'></script>
    <style>
        #wrapper {
          width: 80vw;
          height: auto;
          margin: 0 auto;
          text-align: center;
        }
    </style>
</head>
<body>
    <div id='wrapper'>
        <canvas id='myChart'></canvas>
    </div>
    <script>
        var ctx = document.getElementById('myChart').getContext('2d');
        var myChart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: [],
                datasets: [{
                    label: 'Dolžina (cm)',
                    data: [],
                    borderColor: 'rgba(75, 192, 192, 1)',
                    backgroundColor: 'rgba(173, 216, 230, 0.4)',
                    borderWidth: 2,
                    fill: true
                }]
            },
            options: {
                scales: {
                    x: {
                        title: {
                            display: true,
                            text: 'Čas (s)'
                        },
                        ticks: {
                          autoSkip: false
                        }
                    },
                    y: {
                        beginAtZero: true,
                        title: {
                            display: true,
                            text: 'Dolžina (cm)'
                        }
                    }
                }
            }
        });

        function fetchData() {
            fetch('/data')
                .then(response => response.json())
                .then(data => {
                    myChart.data.labels = data.labels;
                    myChart.data.datasets[0].data = data.readings;
                    myChart.update();
                });
        }

        fetchData();
        setInterval(fetchData, 5000);
    </script>
</body>
</html>
)rawliteral";

#endif // HTML_CONTENT_H
