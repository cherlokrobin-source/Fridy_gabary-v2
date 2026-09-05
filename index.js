export default {
  async fetch(request) {
    const corsHeaders = {
      "Access-Control-Allow-Origin": "*",
      "Access-Control-Allow-Methods": "GET, POST, OPTIONS",
      "Access-Control-Allow-Headers": "Content-Type",
      "Content-Type": "application/json; charset=utf-8"
    };

    if (request.method === "OPTIONS") {
      return new Response(null, { headers: corsHeaders });
    }

    const url = new URL(request.url);
    let year = parseInt(url.searchParams.get("year") || "2026");
    let month = parseInt(url.searchParams.get("month") || "9");
    let day = parseInt(url.searchParams.get("day") || "5");

    let y = year, m = month, d = day;
    if (m <= 2) { y -= 1; m += 12; }
    let a = Math.floor(y / 100);
    let b = 2 - a + Math.floor(a / 4);
    let jdn = Math.floor(365.25 * (y + 4716)) + Math.floor(30.6001 * (m + 1)) + d + b - 1524;

    const responseData = {
      status: "success",
      engine: "Gabary V2 Engine",
      input: { year, month, day },
      metrics: {
        jdn: jdn,
        lunar_sync_ratio: ((day * 0.032) % 1.0).toFixed(4),
        solar_sync_ratio: (((day + 10) * 0.025) % 1.0).toFixed(4)
      },
      timestamp: new Date().toISOString()
    };

    return new Response(JSON.stringify(responseData, null, 2), {
      headers: corsHeaders
    });
  }
};
